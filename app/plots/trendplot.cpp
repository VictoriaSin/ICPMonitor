#include "trendplot.h"
#include "plots/labelmarkitem.h"
#include "gui/gui_funcs.h"
#include "gui/dialogWindows/texteditdialog.h"
#include "controller/labels/label.h"

TrendPlot::TrendPlot(QWidget *parent) :
    AbstractCustomPlot(GraphType::TrendGraph, parent),
    mTextEditDialog(new TextEditDialog(this))
{
    // Размер окна ввода
    mTextEditDialog->setMinimumSize(QSize(480, 360));

    // Настраиваем приближение и движение
    axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);
    axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    // Включаем оптимизацию отображения меток
    setAdaptiveSamplingLabelItems(true);

    // Настраиваем цвет отрисовки
    QColor color(51, 242, 67, 200); // Светло-Зелёный

    // Подготавливаем кисть
    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);

    // Подготавливаем ручку
    QPen pen;
    pen.setBrush(brush);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);

    // Создаём график с накапливаемыми средними значениями и настраиваем
    mAverageGraph = addGraph();
    mAverageGraph->setLineStyle(QCPGraph::lsLine);
    mAverageGraph->setPen(pen);
    mAverageGraph->setBrush(brush);
    mAverageGraph->setAdaptiveSampling(true);

    // Настраиваем отображение времени по оси X (нижней)
    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("hh:mm:ss");
    xAxis->setTicker(timeTicker);

    // Создаём две ограничительные линии
    mLowerAlarmLimit = new QCPItemStraightLine(this);
    mUpperAlarmLimit = new QCPItemStraightLine(this);

    // Ручка для отрисовки уровней тревоги
    QPen whiteDotPen;
    whiteDotPen.setColor(QColor(255, 255, 255));
    whiteDotPen.setStyle(Qt::DotLine);
    whiteDotPen.setWidth(2);

    // Устанавливаем ручку для отрисовки уровней тревоги
    mLowerAlarmLimit->setPen(whiteDotPen);
    mUpperAlarmLimit->setPen(whiteDotPen);

    // Добавляем ограничительные линии на новый слой
    addLayer("lineLayer", nullptr, limAbove);
    layer("lineLayer")->setMode(QCPLayer::lmBuffered);
    mLowerAlarmLimit->setLayer("lineLayer");
    mUpperAlarmLimit->setLayer("lineLayer");

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

    // Разрешаем зумить и двигать по осям
    axisRect()->setRangeDragAxes(QList<QCPAxis *>({xAxis, xAxis2, yAxis}));
    axisRect()->setRangeZoomAxes(QList<QCPAxis *>({xAxis, xAxis2, yAxis}));

    // Связываем изменение интервала оси X (нижней) и оси X2 (верхней)
    connect(xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), xAxis2, QOverload<const QCPRange&>::of(&QCPAxis::setRange));

    // Связываем изменение интервала оси X (нижней) c оптимизацией отображения меток
    connect(xAxis, QOverload<const QCPRange&, const QCPRange&>::of(&QCPAxis::rangeChanged), this, [this](const QCPRange &newRange, const QCPRange &oldRange) {
        // Если ось приблизили
        if (newRange.size() != oldRange.size()) {
            AbstractCustomPlot::labelItemsOptimization();
        }
    });

    // Обработка клика по элементам
    connect(this, &TrendPlot::itemClick, this, &TrendPlot::itemClicked);

    retranslate();
}

TrendPlot::~TrendPlot()
{

}

void TrendPlot::setDataContainerForGraph(QSharedPointer<QCPGraphDataContainer> data)
{
    mAverageGraph->setData(data);
}

void TrendPlot::setLabelManager(LabelManager *labelManager)
{
    mLabelManager = labelManager;
}

QSharedPointer<QCPGraphDataContainer> TrendPlot::data() const
{
    return mAverageGraph->data();
}

void TrendPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("время"));
    yAxis->setLabel(tr("мм рт ст"));

    mTextEditDialog->retranslate();
    mTextEditDialog->setWindowTitle(tr("Информация о метке"));
}

void TrendPlot::setUpperAlarmLevelLine(double upperAlarmLimit)
{
    mUpperAlarmLimit->point1->setCoords(0, upperAlarmLimit);
    mUpperAlarmLimit->point2->setCoords(xAxis->range().upper, upperAlarmLimit);
}

void TrendPlot::setLowerAlarmLevelLine(double lowerAlarmLimit)
{
    mLowerAlarmLimit->point1->setCoords(0, lowerAlarmLimit);
    mLowerAlarmLimit->point2->setCoords(xAxis->range().upper, lowerAlarmLimit);
}

void TrendPlot::enableUpperAlarm(bool enable)
{
    mUpperAlarmLimit->setVisible(enable);
}

void TrendPlot::enableLowerAlarm(bool enable)
{
    mLowerAlarmLimit->setVisible(enable);
}

void TrendPlot::itemClicked(QCPAbstractItem *item, QMouseEvent *event)
{
    const LabelMarkItem *labelItem = dynamic_cast<LabelMarkItem*>(item);

    // Если преобразование прошло успешно и событие отпускания левой мышки
    if (labelItem && event->type() == QEvent::Type::MouseButtonRelease &&
            event->button() == Qt::MouseButton::LeftButton) {
        // Метка, связанная с отображение на графике
        Label *label = labelItem->getLabel();

        // Если метки нет
        if (!label) {
            return;
        }

        // Время начала и окончания метки
        const int64_t &timeStartLabelMs = label->getTimeStartLabelMS();
        const int64_t &timeEndLabelMs = label->getTimeEndLabelMS();

        // Устанавливаем id сессии, к которой относится метка,
        // номер метки, время начала метки и окончание
        mTextEditDialog->setInfoLabelText(TemplateInfoAboutLabel.arg(tr("ID сессии")).arg(label->getBelongIdSession()).
                                          arg(tr("Номер метки")).arg(label->getNumberLabel()).
                                          arg(tr("Время начала метки")).arg(QDateTime::fromMSecsSinceEpoch(timeStartLabelMs).toString("dd.MM.yyyy hh:mm:ss.zzz")).
                                          arg(tr("Время окончания метки")).arg(timeEndLabelMs > timeStartLabelMs ? QDateTime::fromMSecsSinceEpoch(timeEndLabelMs).toString("dd.MM.yyyy hh:mm:ss.zzz") : "-").
                                          arg(tr("Информация")));

        // Информация о метке
        const QString &labelInfo = label->getInfoLabel();

        // Устанавливаем информацию о метке
        mTextEditDialog->setText(labelInfo);

        // Если пользователь подтвердил изменения
        if (mTextEditDialog->exec() == QDialog::Accepted) {
            // Запоминаем текст из поля ввода
            const QString newText = mTextEditDialog->getText();

            // Если изменения были, обновляем информацию о метке
            if (labelInfo != newText) {
                label->setLabelInfo(newText);
            }
        }
    }
}

void TrendPlot::resetGraph()
{
    mAverageGraph->data()->clear();
    qDebug() << "TrendPlot reset";
}

void TrendPlot::scaleFont(double scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
    mTextEditDialog->scaleFont(scaleFactor);
}
