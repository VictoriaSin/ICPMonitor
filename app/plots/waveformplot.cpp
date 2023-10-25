#include "waveformplot.h"
#include "gui/gui_funcs.h"

WaveFormPlot::WaveFormPlot(QWidget *parent) :
    AbstractCustomPlot(GraphType::WaveFormGraph, parent)
{
    // Настраиваем приближение и движение только по вертикали
    //axisRect()->setRangeDrag(Qt::Horizontal);
    //axisRect()->setRangeZoom(Qt::Vertical);
    setInteraction(QCP::iRangeDrag, false);
    setInteraction(QCP::iRangeZoom, false);

    // Добавляем два графика
    mMainGraph = addGraph();
    mHistGraph = addGraph();
    mTempGraph = addGraph();
    mHistTempGraph = addGraph();

    // Включаем сглаживание
    mMainGraph->setAntialiased(true);
    mHistGraph->setAntialiased(true);

    // Метки осей (названия) будут кэшированы
    //setPlottingHint(QCP::PlottingHint::phCacheLabels, true);

    // Настраиваем ручку для отрисовки графика
    QColor color(255, 255, 0); // Жёлтый

    // Подготавливаем кисть
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(mThicknessOfMainGraph);

    // Устанавливаем браш и ручку для отрисовки основного графика
    mMainGraph->setPen(pen);
    mMainGraph->setBrush(brush);

    // Настраиваем ручку для исторического графика
    pen.setWidthF(mThicknessOfHistGraph);

    // Устанавливаем браш и ручку для отрисовки исторического графика
    mHistGraph->setPen(pen);
    mHistGraph->setBrush(brush);

    pen.setColor(Qt::GlobalColor::gray);
    mHistTempGraph->setPen(pen);
    mHistTempGraph->setBrush(brush);

    pen.setWidthF(mThicknessOfMainGraph);
    mTempGraph->setPen(pen);
    mTempGraph->setBrush(brush);

    mAmplitudePoints = addGraph();
    mAmplitudePoints->setPen(QPen(QColor(0, 255, 0)));
    mAmplitudePoints->setLineStyle(QCPGraph::lsNone);
    mAmplitudePoints->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    // Создаём две ограничительные линии
    mLowerAlarmLimit = new QCPItemLine(this);
    mUpperAlarmLimit = new QCPItemLine(this);

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

    // Соединяем изменение диапазона оси X с проверкой вхождения в интервал
    //connect(xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(checkXAxisInterval(QCPRange)));

    retranslate();
}

WaveFormPlot::~WaveFormPlot()
{

}

void WaveFormPlot::setType(uint8_t newType)
{
    type = newType;
    retranslate();
}

void WaveFormPlot::changePenColor(QColor mColor)
{
    QPen pen;
    pen.setColor(mColor);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(mThicknessOfMainGraph);
    mMainGraph->setPen(pen);
    pen.setWidthF(mThicknessOfHistGraph);
    mHistGraph->setPen(pen);
}


void WaveFormPlot::addDataOnGraphic(unsigned int  x, /*unsigned int*/float  y)//const ComplexValue &complexVal)
{
  if (x > graphCurrentMaxRange)
  {
      *mHistGraph->data() = *mMainGraph->data();
      mMainGraph->data()->clear();
      if (type == 0)
      {
          *mHistTempGraph->data() = *mTempGraph->data();
          mTempGraph->data()->clear();
          mAmplitudePoints->data()->clear();
      }

      graphCurrentMaxRange += graphRangeSize;
      graphMinus += graphRangeSize;
  }

    x-= graphMinus;
    float temp_x = (float) x/1000;
    //float temp_y = (float) y;


    if(mHistGraph->data()->size())
    {
        mHistGraph->data()->removeBefore(temp_x + 0.5);
        if (type == 0)
        {
            mHistTempGraph->data()->removeBefore(temp_x + 0.5);
        }
        //mAmplitudePoints->data()->removeAfter(temp_x);

        //mMainGraph->addData(temp_x, temp_y);
    }
    mMainGraph->addData(temp_x, /*temp_y*/y);
}

void WaveFormPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("секунды"));
    if (type == 0) { mICPSettings->getCurrentPressureIndex() == 0 ? yAxis->setLabel(tr("мм рт ст")) : yAxis->setLabel(tr("мм вод ст")); }
    else if (type == 1) { yAxis->setLabel(tr("Compliance")); }
}

void WaveFormPlot::resetGraph()
{
    mPreviousSensorDataTime = 0;
    mSummarySensorDataTimePerXRange = 0;
    mMainGraph->data()->clear();
    mHistGraph->data()->clear();
    qDebug() << "WavePlot reset";
}

void WaveFormPlot::scaleFont(float scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
}

void WaveFormPlot::setUpperAlarmLevelLine(float upperAlarmLimit)
{
    mUpperAlarmLimit->start->setCoords(xAxis->range().lower, upperAlarmLimit);
    mUpperAlarmLimit->end->setCoords(xAxis->range().upper, upperAlarmLimit);
}

void WaveFormPlot::setLowerAlarmLevelLine(float lowerAlarmLimit)
{
    mLowerAlarmLimit->start->setCoords(xAxis->range().lower, lowerAlarmLimit);
    mLowerAlarmLimit->end->setCoords(xAxis->range().upper, lowerAlarmLimit);
}

void WaveFormPlot::enableUpperAlarm(bool enable)
{
    mUpperAlarmLimit->setVisible(enable);
}

void WaveFormPlot::enableLowerAlarm(bool enable)
{
    mLowerAlarmLimit->setVisible(enable);
}


//qDebug() << temp_x << temp_y;
//    // Суммирование общего времени пришедших данных с датчика
//    // для ограничения отображения данных в диапазоне допустимых
//    // значение времени на оси Х графика
//    if (mPreviousSensorDataTime == 0)
//    {
//        mPreviousSensorDataTime = complexVal.timestamp;
//    }
//    else
//    {
//        mSummarySensorDataTimePerXRange += (complexVal.timestamp - mPreviousSensorDataTime) / 1000.0;
//    }

//    // Если суммарное время пришедних данных стало больше ограничения
//    // установленного осью Х графика
//    if(mSummarySensorDataTimePerXRange >= xAxis->range().upper) //mCurrentMaxXRange){
//    {
//        *mHistGraph->data() = *mMainGraph->data();
//        mMainGraph->data()->clear();
//        mSummarySensorDataTimePerXRange = 0;
//        mPreviousSensorDataTime = 0;
//    }

//    //#ifdef QT_DEBUG
//    if (avgBenchTime > 1000) {
//        //qDebug() << "Gui update period, ms" << avgBenchTime / (float)benchCount;
//        avgBenchTime = 0;
//        benchCount = 0;
//    }
//    //#endif

//    // Добавляем на текущий график данные
//    mMainGraph->addData(mSummarySensorDataTimePerXRange, complexVal.value);

//    // Удаляем с предыдущего графика точку
//    if(mHistGraph->data()->size()){
//        mHistGraph->data()->removeBefore(mSummarySensorDataTimePerXRange + 0.5);
//    }

//    mPreviousSensorDataTime = complexVal.timestamp;
