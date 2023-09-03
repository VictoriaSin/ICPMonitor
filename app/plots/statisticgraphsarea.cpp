#include "statisticgraphsarea.h"
#include "ui_statisticgraphsarea.h"
#include "ui_abstractgraphareawidget.h"
#include "controller/monitorcontroller.h"
#include "statistical_functions.h"
#include "gui/gui_funcs.h"

#include <limits>
#include <QThreadPool>
#include <QtConcurrent>

#define INDENT_FACTOR 50

StatisticGraphsArea::StatisticGraphsArea(QWidget *parent) :
    AbstractMultipleGraphsAreasWidget(parent),
    ui(new Ui::StatisticGraphsArea)
{
    ui->setupUi(AbstractGraphAreaWidget::ui->widget);

    // Записываем для быстрого доступа
    mBoxGraph = ui->boxPlot;

    // Добавляем графики в общий контейнер
    mGraphContainer.append(mBoxGraph);

    // Указываем индекс отображаемого графика
    mCurrentGraphIndex = 0;
    mDisplayedGraph = mGraphContainer[mCurrentGraphIndex]->type();

    // Заполняем контейнер масштабов, кол-во коробок присущих масштабу, и инициализируем кнопку
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(3600, QIcon(":/icons/oneHour_pressed.svg"), QIcon(":/icons/oneHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(7200, QIcon(":/icons/twoHour_pressed.svg"), QIcon(":/icons/twoHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(10800, QIcon(":/icons/threeHour_pressed.svg"), QIcon(":/icons/threeHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(14400, QIcon(":/icons/fourHour_pressed.svg"), QIcon(":/icons/fourHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(28800, QIcon(":/icons/eightHour_pressed.svg"), QIcon(":/icons/eightHour.svg")));
    mXRangesOfSecondsWithIcons.append(IntervalAndButtonIcons(43200, QIcon(":/icons/twelveHour_pressed.svg"), QIcon(":/icons/twelveHour.svg")));
    initializeXRangeToolButton();

    // Устанавливаем диапазон относительно текущего времени
    int64_t currentTimeSeconds = QDateTime::currentSecsSinceEpoch();
    mBoxGraph->setXRange(currentTimeSeconds - mXRangesOfSecondsWithIcons[mCurrentXRangeIndex].interval, currentTimeSeconds);

    // Изменения диапазона графиков по оси X
    connect(AbstractGraphAreaWidget::ui->xRangeGraphToolButton, &QToolButton::clicked, this, &StatisticGraphsArea::nextXRange);

    // Изменения диапазона графиков по оси Y
    connect(AbstractGraphAreaWidget::ui->yRangeGraphToolButton, &QToolButton::clicked, this, &StatisticGraphsArea::nextYRange);

    // Настраиваем кнопку для обновления текущего диапазона
    setupUpdateCurrentRangeData();

    // Устанавливаем наблюдателя за окончанием вычисления коробок для диаграммы
    connect(&mFutureWatcher, &QFutureWatcher<QCPStatisticalBoxDataContainer>::finished, this, &StatisticGraphsArea::responseUpdateRequestBoxGraph);
}

StatisticGraphsArea::~StatisticGraphsArea()
{
    delete ui;
}

bool StatisticGraphsArea::nextXRange()
{
    // Если не у кого запросить данные или нет графика
    if (!mController || !mBoxGraph) {
        return false;
    }

    // Кол-во доступных диапазонов
    const int countIntervals = mXRangesOfSecondsWithIcons.size();

    // Если в списке нет диапазонов
    if (countIntervals == 0) {
        return false;
    }

    // Переключаем индекс диапазонов
    ++mCurrentXRangeIndex;
    mCurrentXRangeIndex %= countIntervals;
    const auto &intervalAndIcon = mXRangesOfSecondsWithIcons[mCurrentXRangeIndex];

    // Текущее время в секундах
    int64_t currentTimeSeconds = QDateTime::currentSecsSinceEpoch();

    // Меняем диапазон по оси X
    mBoxGraph->setXRange(currentTimeSeconds - intervalAndIcon.interval, currentTimeSeconds);

    // Устанавливаем картинку кнопке
    AbstractGraphAreaWidget::ui->xRangeGraphToolButton->setIcon(intervalAndIcon.defaultButtonIcon, intervalAndIcon.pressedButtonIcon);

    // Обновляем данные на выставленном диапазоне
    return updateBoxGraphOnCurrentRange();
}

bool StatisticGraphsArea::updateBoxGraphOnCurrentRange()
{
    if (!mBoxGraph) {
        return false;
    }

    // Очищаем данные прошлых коробок
    mBoxGraph->getDataContainer()->clear();

    // Перестраиваем сразу же, чтобы узнать ключевые точки тиков
    mBoxGraph->replot(QCustomPlot::RefreshPriority::rpImmediateRefresh);

    // Получаем список тиков
    const auto ticks = mBoxGraph->xAxis->tickVector();
    if (ticks.size() < 2) {
        return false;
    }

    // Узнаём интервал между тиками
    const double ticksInterval = ticks.at(1) - ticks.at(0);

    // Вычисляем границы для рассчета коробок
    QCPRange rangeForCalculatingBoxes;
    rangeForCalculatingBoxes.lower = ticks.constFirst() - ticksInterval;
    rangeForCalculatingBoxes.upper = ticks.constLast() + ticksInterval;

    return updateRequestBoxGraphData(rangeForCalculatingBoxes, ticks.size() + 1);
}

void StatisticGraphsArea::responseUpdateRequestBoxGraph()
{
    mRequestedData.clear(); // Освобождаем RAM

    if (mFuture.resultCount() == 0) {
        return;
    }

    updateBoxGraph(mFuture.result());
}

void StatisticGraphsArea::updateBoxGraph(const QCPStatisticalBoxDataContainer &data)
{
    // Устанавливаем данные графику
    mBoxGraph->setDataContainer(QSharedPointer<QCPStatisticalBoxDataContainer>(new QCPStatisticalBoxDataContainer(data)));

    // Масштабируем коробки
    mBoxGraph->scaleBoxesRelativeToDataRange();

    // Обновляем диапазон Y в соответствии с текущим режимом
    updateCurrentYRange();

    // Перестраиваем график
    mBoxGraph->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

bool StatisticGraphsArea::updateRequestBoxGraphData(const QCPRange &xRange, int8_t countBox)
{
    if (!mBoxGraph || !mController || mFuture.isRunning()) {
        return false;
    }

    // Запрашиваем у контроллера данные из бинарных файлов за текущую сессию
    mRequestedData = mController->getAverageSensorReadingsFromTheIntervalForTheCurrentSession(xRange.lower, xRange.upper).toStdVector();

    // Если данных нет
    if (mRequestedData.size() == 0) {
        return false;
    }

    // Скидываем вычисления коробок в другой доступный поток из пула
    mFuture = QtConcurrent::run(QThreadPool::globalInstance(), BoxPlot::calculateBoxGraph, &this->mRequestedData, xRange, countBox, true);
    mFutureWatcher.setFuture(mFuture);

    return true;
}

void StatisticGraphsArea::nextYRange()
{
    // Переключаем режим
    mYZoomModeBoxGraph = !mYZoomModeBoxGraph;

    // Если новый диапазон не установлен, возвращаем предыдущий режим
    if (!updateCurrentYRange()) {
        mYZoomModeBoxGraph = !mYZoomModeBoxGraph;
    }
}

bool StatisticGraphsArea::updateCurrentYRange()
{
    // Результат установки нового диапазона
    bool isOkey {false};

    // Приближаем в соответствии с режимом
    if (mYZoomModeBoxGraph) {
        isOkey = updateYRange(BoxPlot::arrangeAllTheBoxesWithWhiskersInside);
    } else {
        isOkey = updateYRange(BoxPlot::arrangeAllTheBoxesInside);
    }

    return isOkey;
}

bool StatisticGraphsArea::updateYRange(void (*func)(QCPStatisticalBoxDataContainer *, double &, double &))
{
    // Если нет функции или нет графика коробчатой диаграммы
    if (!func || !mBoxGraph) {
        return false;
    }

    // Получаем контейнер с данными
    const auto& data = mBoxGraph->getDataContainer();

    // Если нет контейнера или в контейнере нет коробок
    if (!data.get() || data->isEmpty()) {
        return false;
    }

    // Будущий диапазон
    double lower = std::numeric_limits<double>::max();
    double upper = std::numeric_limits<double>::min();

    // Ищем минимальное и максимальное значение диапазона в соответствии с принятой функцией
    func(data.get(), lower, upper);

    // Если хоть одно значение из диапазона не заполнилось
    if (lower == std::numeric_limits<double>::max() || upper == std::numeric_limits<double>::min()) {
        return false;
    }

    // Рассчитываем отступ для верха и низа
    const double margin = (upper - lower) / INDENT_FACTOR;

    // Устанавливаем новый диапазон по оси Y
    mBoxGraph->setYRange(lower - margin, upper + margin);

    return true;
}

void StatisticGraphsArea::showEvent(QShowEvent *event)
{
    AbstractMultipleGraphsAreasWidget::showEvent(event);

    // Обновляем текущий диапазон коробчатыми диаграммами
    updateBoxGraphOnCurrentRange();
}

void StatisticGraphsArea::replotDisplayedGraph()
{
    if (!mBoxGraph) {
        return;
    }

    switch (mDisplayedGraph) {
    case AbstractCustomPlot::GraphType::BoxGraph: {
        mBoxGraph->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
        break;
    }
        default: break;
    }
}

void StatisticGraphsArea::installController(MonitorController *controller)
{
    AbstractGraphAreaWidget::installController(controller);
}

void StatisticGraphsArea::retranslate()
{
    AbstractMultipleGraphsAreasWidget::retranslate();
    ui->retranslateUi(this);
    if (mBoxGraph) {
        mBoxGraph->retranslate();
    }
}

void StatisticGraphsArea::scaleFont(float scaleFactor)
{
    AbstractMultipleGraphsAreasWidget::scaleFont(scaleFactor);
    if (mBoxGraph) {
        mBoxGraph->scaleFont(scaleFactor);
    }
}

void StatisticGraphsArea::setupUpdateCurrentRangeData()
{
    // Если кнопка уже создана
    if (mUpdateRangeToolButton) {
        return;
    }

    // Настраиваем кнопку для переключения между статистическими графиками
    mUpdateRangeToolButton = new AnimatedToolButton(this);
    mUpdateRangeToolButton->setIcon(QIcon(":/icons/reload.svg"), QIcon(":/icons/reload_pressed.svg"));
    mUpdateRangeToolButton->setIconSize(QSize(90, 90));
    mUpdateRangeToolButton->setStyleSheet(readStyleSheetFile(":/styles/ToolButtons.qss"));

    // Добавляем кнопку для переключения между статистическими графиками в левый layout
    addWidgetOnLeftVerticalLayout(mUpdateRangeToolButton);

    // Обновление текущего диапазона
    connect(mUpdateRangeToolButton, &QToolButton::clicked, this, &StatisticGraphsArea::updateBoxGraphOnCurrentRange);
}
