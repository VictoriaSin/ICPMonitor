#include "intervalplot.h"
#include "recordedplot.h"
#include "gui/gui_funcs.h"

#include "markitem.cpp"

extern QFile mRawDataFile;
extern QFile mMarksFile;

QPair<float, float> mFirstIntervalMinMaxXRange = qMakePair(0, 60);
QPair<float, float> mSecondIntervalMinMaxXRange = qMakePair(0, 60);

IntervalPlot::IntervalPlot(QWidget *parent):
    AbstractCustomPlot(GraphType::IntervalGraph, parent)
{
    mMainGraph = addGraph();
    mMainGraph->setAntialiased(true);
    addLayer("lineLayer", nullptr, limAbove);
    layer("lineLayer")->setMode(QCPLayer::lmBuffered);
    mICPSettings->getCurrentPressureIndex() == 0 ? mCurrentMaxYRange = 60 : mCurrentMaxYRange = 815.7;
    setInteraction(QCP::iRangeDrag, true); // Можем передвигать график
    setInteraction(QCP::iRangeZoom, true); // Можем приближать график
    retranslate();    
}

//IntervalPlot::~IntervalPlot()
//{
//    //
//}



void IntervalPlot::setup(QPair<int, int> points, QColor color)
{
    //настройка ручки
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(1.7); // толщина линии

    // Устанавливаем браш и ручку для отрисовки основного графика
    mMainGraph->setPen(pen);
    mMainGraph->setBrush(brush);

    points.first= (points.first - (points.first % 2)) * 3;
    points.second = (points.second  - (points.second % 2)) * 3 ;

    qDebug() << "start1" << points.first << "stop2" << points.second;

    iterTemp = ((points.second-points.first)/6) + 1;

    _mSPIData *tempArr = new _mSPIData[iterTemp];
    mRawDataFile.open(QIODevice::ReadOnly);
    mRawDataFile.seek(points.first);
    mRawDataFile.read((char*)tempArr, iterTemp*6);
    mRawDataFile.close();

    for (uint i=0; i<iterTemp; i++)
    {
        mMainGraph->addData((float)tempArr[i].timeStamp/1000, tempArr[i].data);
    }
    qDebug() << "mCurrentIntervalNum" << mCurrentIntervalNum;
    if (mCurrentIntervalNum == 1)
    {
        mFirstIntervalMinMaxXRange.first = (float)tempArr[0].timeStamp/1000;
        mFirstIntervalMinMaxXRange.second = (float)tempArr[iterTemp-1].timeStamp/1000;
        qDebug() << "tttt" << mFirstIntervalMinMaxXRange.first << mFirstIntervalMinMaxXRange.second;
    }
    else
    {
        mSecondIntervalMinMaxXRange.first = (float)tempArr[0].timeStamp/1000;
        mSecondIntervalMinMaxXRange.second = (float)tempArr[iterTemp-1].timeStamp/1000;
        qDebug() << "tttt" << mSecondIntervalMinMaxXRange.first << mSecondIntervalMinMaxXRange.second;
    }

    xAxis->setRange((float)tempArr[0].timeStamp/1000, (float)tempArr[iterTemp-1].timeStamp/1000);
    delete[] tempArr;
}


void IntervalPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("секунды"));
    mICPSettings->getCurrentPressureIndex() == 0 ? yAxis->setLabel(tr("мм рт ст")) : yAxis->setLabel(tr("мм вод ст"));

}

void IntervalPlot::scaleFont(float scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
}

bool IntervalPlot::event(QEvent *event)
{
    // сенсор
    return AbstractCustomPlot::event(event);
}

