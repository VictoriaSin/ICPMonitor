#include "recordedplot.h"
#include "plots/labelmarkitem.h"
#include "gui/gui_funcs.h"
#include "gui/dialogWindows/texteditdialog.h"
#include "controller/labels/label.h"
#include "global_define.h"
//LabelManager *mLabelManagerRecorded {nullptr};


RecordedPlot::RecordedPlot(QWidget *parent):
    AbstractCustomPlot(GraphType::RecordedGraph, parent),
    mTextEditDialog(new TextEditDialog(this))
{
    // Размер окна ввода
    mTextEditDialog->setMinimumSize(QSize(480, 360));
    isLabelCreating = false;
    isIntervalCreating = false;
    // Настраиваем приближение и движение
    axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    // Включаем оптимизацию отображения меток
    setAdaptiveSamplingLabelItems(true);
    mMainGraph = addGraph();
    mMainGraph->setAntialiased(true);

    mMainGraph->setAdaptiveSampling(true);//&

    // Настраиваем ручку для отрисовки графика
    QColor color(51, 242, 67, 200); // зеленый

    // Подготавливаем кисть
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(mThicknessOfMainGraph);

    // Устанавливаем браш и ручку для отрисовки основного графика
    mMainGraph->setPen(pen);
    mMainGraph->setBrush(brush);

    addLayer("lineLayer", nullptr, limAbove);
    layer("lineLayer")->setMode(QCPLayer::lmBuffered);

    // Соединяем изменение диапазона оси X с проверкой вхождения в интервал
    //connect(xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(checkXAxisInterval(QCPRange)));

//    // Разрешаем зумить и двигать по осям
//    axisRect()->setRangeDragAxes(QList<QCPAxis *>({xAxis, xAxis2, yAxis}));
//    // Связываем изменение интервала оси X (нижней) и оси X2 (верхней)
//    connect(xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged), xAxis2, QOverload<const QCPRange&>::of(&QCPAxis::setRange));

    // Связываем изменение интервала оси X (нижней) c оптимизацией отображения меток
    connect(xAxis, QOverload<const QCPRange&, const QCPRange&>::of(&QCPAxis::rangeChanged), this, [this](const QCPRange &newRange, const QCPRange &oldRange) {
        // Если ось приблизили
        if (newRange.size() != oldRange.size())
        {
            AbstractCustomPlot::labelItemsOptimization();
        }
    });

    // Обработка клика по элементам
    connect(this, &RecordedPlot::itemClick, this, &RecordedPlot::itemClicked);

    retranslate();
    addLayer("intervalLayer", nullptr, limAbove);
}

RecordedPlot::~RecordedPlot()
{
    mRecordedData.clear();
}

void RecordedPlot::addInterval(uint8_t num, QColor color)
{
    mIntervalFirst = addGraph();
    mIntervalFirst->setAntialiased(true);
    mIntervalFirst->setAdaptiveSampling(true);//?

    // Подготавливаем кисть
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(mThicknessOfMainGraph);

    // Устанавливаем браш и ручку для отрисовки основного графика
    mIntervalFirst->setPen(pen);
    mIntervalFirst->setBrush(brush);


    layer("intervalLayer")->setMode(QCPLayer::lmBuffered);

    QVector<double> temp;
    for (int i=0; i<mRecordedData.count(); i++)
    {
        temp.push_back(mRecordedData[i].first);
    }

    double t1 = (double)mIntervalsContainer[num-2]->mIntervalPos/1000.0;
    double t2 = (double)mIntervalsContainer[num-1]->mIntervalPos/1000.0;

    double first = 0.0;
    double second = 0.0;
    for (int i=floor(t1)*25; i<=ceil(t2)*25; i++) //25 показаний в секунду
    {
        if (mRecordedData[i].first <= t1)
        {
            first = mRecordedData[i].first;
        }
        else if (mRecordedData[i].first >= t2)
        {
            second = mRecordedData[i].first;
            break;
        }

    }

    int indexStart = temp.indexOf(first);
    int indexStop = temp.indexOf(second);
    qDebug() << t1 << indexStart;
    qDebug() << t2 << indexStop;

    for (int i=indexStart; i<=indexStop; i++)
    {
        mIntervalFirst->addData(mRecordedData[i].first, mRecordedData[i].second);
    }
}

void RecordedPlot::saveDataForGraphic(const ComplexValue &complexVal)
{
    // Суммирование общего времени пришедших данных с датчика
    // для ограничения отображения данных в диапазоне допустимых
    // значение времени на оси Х графика

    if (mPreviousSensorDataTime == 0)
    {
        mPreviousSensorDataTime = complexVal.timestamp;
    }
    else
    {
        mSummarySensorDataTimePerXRange += (complexVal.timestamp - mPreviousSensorDataTime) / 1000.0;
    }

//    //#ifdef QT_DEBUG
//    if (avgBenchTime > 1000) {
//        //qDebug() << "Gui update period, ms" << avgBenchTime / (float)benchCount;
//        avgBenchTime = 0;
//        benchCount = 0;
//    }
//    //#endif
    mRecordedData.push_back(qMakePair(mSummarySensorDataTimePerXRange, complexVal.value));
    mPreviousSensorDataTime = complexVal.timestamp;

}

void RecordedPlot::addDataOnGraphic()
{
    double mNewUpperXValue = (double)mRecordedData.count()/25.0;// показаний в файле в секунду 50, а мы берем каждое второе
    mUpper = xAxis->range().upper;
    if (mNewUpperXValue < mUpper)
    {
        xAxis->setRangeUpper(mNewUpperXValue);
        setInteraction(QCP::iRangeDrag, false);
    }
    mCurrentMaxXRange = mNewUpperXValue;
    for (int i = 0; i < mRecordedData.count(); i++)
    {
        mMainGraph->addData(mRecordedData[i].first, mRecordedData[i].second);
    }
}

void RecordedPlot::checkXAxisInterval(const QCPRange &range)
{
    if (range.lower < 0)
    {
        xAxis->setRange(0, mUpper);
    }
    else if (range.upper > mCurrentMaxXRange)
    {
        xAxis->setRange(mCurrentMaxXRange - mUpper, mCurrentMaxXRange);
    }
}

//void RecordedPlot::setLabelManager(LabelManager *labelManager)
//{
//    //mLabelManager = labelManager;
//    //mLabelManagerRecorded = labelManager;
//}

void RecordedPlot::itemClicked(QCPAbstractItem *item, QMouseEvent *event)
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

void RecordedPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("секунды"));
    yAxis->setLabel(tr("мм рт ст"));

    mTextEditDialog->retranslate();
    mTextEditDialog->setWindowTitle(tr("Информация о метке"));
}

void RecordedPlot::scaleFont(double scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
    mTextEditDialog->scaleFont(scaleFactor);
}

void RecordedPlot::resetGraph()
{
    mPreviousSensorDataTime = 0;
    mSummarySensorDataTimePerXRange = 0;
    mRecordedData.clear();
    setInteraction(QCP::iRangeDrag, true);
    mCurrentMaxXRange = 60;
    mMainGraph->data()->clear();
    qDebug() << "RecordedPlot reset";
}
