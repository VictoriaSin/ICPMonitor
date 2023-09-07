#include "boxplot.h"
#include "gui/gui_funcs.h"
#include "global_structs.h"
#include "statistical_functions.h"

/*! Не ставить значение ниже двух */
#define MINIMAL_VALUES_FOR_BOXGRAPH 5

BoxPlot::BoxPlot(QWidget *parent) :
    AbstractCustomPlot(GraphType::BoxGraph, parent)
{
    // Настраиваем приближение и движение
    axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);
    axisRect()->setRangeZoom(Qt::Vertical);

    // Настраиваем коробчатую диаграмму
    mBoxGraph = new QCPStatisticalBox(xAxis, yAxis);
    baseBoxDrawing();
    baseBackboneWhiskerDrawing();
    baseWhiskerDrawing();
    baseMedianPenDrawing();
    baseOutlierStyleDrawing();
    mBoxGraph->setWhiskerAntialiased(true); // Отрисовка усов со сглаживанием

    // Устанавливаем тикер с датой
    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("hh:mm:ss");
    timeTicker->setTickCount(6);
    xAxis->setTicker(timeTicker);

    // Настраиваем отображение даты по оси X2 (верхней)
    xAxis2->setVisible(true); // Включаем её отображение
    xAxis2->setTicks(true); // Включаем отображение тиков
    xAxis2->setTickLabels(true); // Включаем отображение подписей
    xAxis2->setTickPen(QPen(QBrush(Qt::BrushStyle::NoBrush), 0, Qt::PenStyle::NoPen)); // Чтобы не отрисовывать тики

    // Добавляем тикер на ось X2 (верхняя)
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("dd.MM.yyyy");
    dateTicker->setTickCount(2);
    xAxis2->setTicker(dateTicker);

    // Связываем изменение интервала оси X (нижней) и оси X2 (верхней)
    connect(xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), xAxis2, QOverload<const QCPRange&>::of(&QCPAxis::setRange));

    retranslate();
}

BoxPlot::~BoxPlot()
{

}

void BoxPlot::setDataContainer(QSharedPointer<QCPStatisticalBoxDataContainer> data)
{
    if (!mBoxGraph) {
        return;
    }

    mBoxGraph->setData(data);
}

QSharedPointer<QCPStatisticalBoxDataContainer> BoxPlot::getDataContainer() const
{
    if (!mBoxGraph) {
        return {};
    }

    return mBoxGraph->data();
}

void BoxPlot::setupBoxDrawing(const QPen &frameBoxPen, const QBrush &boxBrush, double boxWidth)
{
    if (!mBoxGraph) {
        return;
    }

    // Устанавливаем графику отрисовку
    mBoxGraph->setPen(frameBoxPen); // Устанавливаем ручку для коробок
    mBoxGraph->setBrush(boxBrush); // Кисть для коробок
    mBoxGraph->setWidth(boxWidth); // Ширина коробок
}

void BoxPlot::baseBoxDrawing()
{
    // Подготавливаем цвет раскрашивания коробки
    QColor boxColor(177, 147, 199, 200); // Пурпурный

    // Подготавливаем кисть для отрисовки коробок
    QBrush boxBrush;
    boxBrush.setColor(boxColor);
    boxBrush.setStyle(Qt::SolidPattern);

    // Подготавливаем цвет отрисовки контура коробки
    QColor frameBoxColor(Qt::white);

    // Подготавливаем ручку отрисовки контура коробки
    QPen frameBoxPen;
    frameBoxPen.setColor(frameBoxColor);
    frameBoxPen.setCapStyle(Qt::SquareCap);
    frameBoxPen.setJoinStyle(Qt::MiterJoin);
    frameBoxPen.setStyle(Qt::SolidLine);
    frameBoxPen.setWidth(1);

    // Устанавливаем
    setupBoxDrawing(frameBoxPen, boxBrush, 400);
}

void BoxPlot::setupBackboneWhiskerDrawing(const QPen &backboneWhiskerPen)
{
    if (!mBoxGraph) {
        return;
    }

    mBoxGraph->setWhiskerPen(backboneWhiskerPen); // Ручка отрисовки остова усов
}

void BoxPlot::baseBackboneWhiskerDrawing()
{
    // Подготавливаем цвет отрисовки остова усов
    QColor backboneWhiskerColor(Qt::white); // Белый

    // Подготавливаем ручку отрисовки остова усов
    QPen backboneWhiskerPen;
    backboneWhiskerPen.setColor(backboneWhiskerColor);
    backboneWhiskerPen.setCapStyle(Qt::SquareCap);
    backboneWhiskerPen.setJoinStyle(Qt::MiterJoin);
    backboneWhiskerPen.setStyle(Qt::DashLine);
    backboneWhiskerPen.setWidth(1);

    // Устанавливаем
    setupBackboneWhiskerDrawing(backboneWhiskerPen);
}

void BoxPlot::setupWhiskerDrawing(const QPen &whiskerBarPen, double whiskerWidth)
{
    if (!mBoxGraph) {
        return;
    }

    mBoxGraph->setWhiskerBarPen(whiskerBarPen); // Ручка отрисовки усов
    mBoxGraph->setWhiskerWidth(whiskerWidth); // Ширина усов
}

void BoxPlot::baseWhiskerDrawing()
{
    // Подготавливаем цвет отрисовки усов
    QColor whiskerBarColor(Qt::white);

    // Подготавливаем ручку отрисовки усов
    QPen whiskerBarPen;
    whiskerBarPen.setColor(whiskerBarColor);
    whiskerBarPen.setCapStyle(Qt::SquareCap);
    whiskerBarPen.setJoinStyle(Qt::MiterJoin);
    whiskerBarPen.setStyle(Qt::SolidLine);
    whiskerBarPen.setWidth(2);

    // Устанавливаем
    setupWhiskerDrawing(whiskerBarPen, 100);
}

void BoxPlot::setupMedianDrawing(const QPen &medianPen)
{
    if (!mBoxGraph) {
        return;
    }

    mBoxGraph->setMedianPen(medianPen); // Ручка отрисовки линии медианы
}

void BoxPlot::baseMedianPenDrawing()
{
    // Подготавливаем цвет отрисовки для отрисовки линии медианы
    QColor medianColor(Qt::white);

    // Подготавливаем ручку для отрисовки линии медианы
    QPen medianPen;
    medianPen.setColor(medianColor);
    medianPen.setCapStyle(Qt::SquareCap);
    medianPen.setJoinStyle(Qt::MiterJoin);
    medianPen.setStyle(Qt::SolidLine);
    medianPen.setWidth(2);

    // Устанавливаем
    setupMedianDrawing(medianPen);
}

void BoxPlot::setupOutlierStyleDrawing(const QCPScatterStyle &scatterStyle)
{
    if (!mBoxGraph) {
        return;
    }

    mBoxGraph->setOutlierStyle(scatterStyle); // Внешний вид точек выброса
}

void BoxPlot::baseOutlierStyleDrawing()
{
    // Подготавливаем цвет отрисовки выбросов
    QColor scatterColor(Qt::red);

    // Подготавливаем ручку отрисовки выбросов
    QPen scatterPen;
    scatterPen.setColor(scatterColor);
    scatterPen.setCapStyle(Qt::SquareCap);
    scatterPen.setJoinStyle(Qt::MiterJoin);
    scatterPen.setStyle(Qt::SolidLine);
    scatterPen.setWidth(1);

    // Настраиваем стиль отрисовки выбросов
    QCPScatterStyle scatterStyle;
    scatterStyle.setShape(QCPScatterStyle::ScatterShape::ssCross); // Крестики
    scatterStyle.setPen(scatterPen); // Ручка отрисовки выборосов

    // Устанавливаем
    setupOutlierStyleDrawing(scatterStyle);
}

void BoxPlot::scaleBoxesRelativeToDataRange()
{
    if (!mBoxGraph) {
        return;
    }

    const int xRangeSize = xAxis->range().size();

    mBoxGraph->setWidth(xRangeSize / mKoeffBoxWidth); // Ширина коробок
    mBoxGraph->setWhiskerWidth(xRangeSize / mKoeffWhiskerWidth); // Ширина усов
}

void BoxPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("время"));
    yAxis->setLabel(tr("мм рт ст"));
}

QCPStatisticalBoxDataContainer BoxPlot::calculateBoxGraph(std::vector<ComplexValue> *data, const QCPRange &xRange, int8_t countBox, bool isSortedByTime)
{
    // Подготавливаем результирующий контейнер
    QCPStatisticalBoxDataContainer container;

    // Если пришёл nullptr или нет данных или коробок меньше одной
    if (!data || data->size() == 0 || countBox < 1) {
        return container;
    }

    // Если данные не отсортированы по времени
    if (!isSortedByTime) {
        std::sort(data->begin(), data->end(), ComplexValue::timestampLessThan);
    }

    // Контейнер с данным для одной коробки
    std::vector<ComplexValue> boxData;

    // Cмещение интервала
    const int64_t shiftMilliseconds = xRange.size() / countBox * 1000;

    // Левая граница поиска значения для коробки
    int64_t leftBoundMs = xRange.lower * 1000;

    // Правая граница поиска значения для коробки
    int64_t upperBoundMs = leftBoundMs + shiftMilliseconds;

    // Правая граница диапазона вычисления коробок
    int64_t upperBoarderMs = xRange.upper * 1000;

    // Для убеждения, что коробка посчиталась
    bool isOkey = false;

    // Подготавливаем данные для коробок и рассчитываем их
    for (const auto &item : qAsConst(*data)) {
        // Если данные дальше правой границы
        if (upperBoarderMs < item.timestamp) {
            break;
        }

        // Если значение попадает в диапазон
        if (leftBoundMs <= item.timestamp && item.timestamp <= upperBoundMs) {
            // Добавляем данные для коробки
            boxData.push_back(item);
            continue;
        }

        // Если данные для рассчёта коробки есть
        if (boxData.size() != 0) {
            // Вычисляем коробку
            QCPStatisticalBoxData box = calculateTheBox(&boxData, isOkey, false);
            box.key = (leftBoundMs / 1000 + upperBoundMs / 1000) / 2; // Ставим по центру интервала
            if (isOkey) {
                container.add(box); // Добавляем рассчитаную коробку
            }
            boxData.clear(); // Очищаем контейнер данных для рассчёта коробки
        }

        // Если значение больше правой границы,
        // то смещаем окно вправо
        if (upperBoundMs < item.timestamp) {
            leftBoundMs += shiftMilliseconds;
            upperBoundMs += shiftMilliseconds;
            continue;
        }
    }

    // Если набрались данные для остаточной коробки
    if (boxData.size() != 0) {
        // Вычисляем коробку
        QCPStatisticalBoxData box = calculateTheBox(&boxData, isOkey, false);
        box.key = (leftBoundMs / 1000 + upperBoundMs / 1000) / 2; // Ставим по центру интервала
        if (isOkey) {
            container.add(box); // Добавляем рассчитаную коробку
        }
    }

    return container;
}

QCPStatisticalBoxData BoxPlot::calculateTheBox(std::vector<ComplexValue> *data, bool &isOkey, bool isSortedByValue)
{
    // Результирующая коробка
    QCPStatisticalBoxData boxData;

    // Если nullptr или кол-во значений меньше MINIMAL_VALUES_FOR_BOXGRAPH
    if (!data || data->size() < MINIMAL_VALUES_FOR_BOXGRAPH) {
        isOkey = false;
        return boxData;
    }

    // Сортируем по возрастанию для поиска статистических параметров
    if (!isSortedByValue) {
        std::sort(data->begin(), data->end(), ComplexValue::valueLessThan);
    }

    // Вычисляем минимальное значение
    boxData.minimum = data->begin()->value;

    // Вычисляем максимальное значение
    boxData.maximum = data->rbegin()->value;

    // Вычисляем медиану
    boxData.median = calculateMedian(data, isOkey, true);

    // Вычисляем нижний квартиль
    boxData.lowerQuartile = calculateLowerQuartile(data, isOkey, true);

    // Вычисляем верхний квартиль
    boxData.upperQuartile = calculateUpperQuartile(data, isOkey, true);

    return boxData;
}

void BoxPlot::arrangeAllTheBoxesWithWhiskersInside(QCPStatisticalBoxDataContainer *data, double &lower, double &upper)
{
    if (!data) {
        return;
    }

    for (auto it = data->begin(); it != data->end(); ++it) {
        // Поиск минимального значения
        if (it->minimum < lower) {
            lower = it->minimum;
        }

        // Поиск максимального значения
        if (it->maximum > upper) {
            upper = it->maximum;
        }
    }
}

void BoxPlot::arrangeAllTheBoxesInside(QCPStatisticalBoxDataContainer *data, double &lower, double &upper)
{
    if (!data) {
        return;
    }

    for (auto it = data->begin(); it != data->end(); ++it) {
        // Поиск минимального значения
        if (it->lowerQuartile < lower) {
            lower = it->lowerQuartile;
        }

        // Поиск максимального значения
        if (it->upperQuartile > upper) {
            upper = it->upperQuartile;
        }
    }
}

void BoxPlot::scaleFont(double scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
}
