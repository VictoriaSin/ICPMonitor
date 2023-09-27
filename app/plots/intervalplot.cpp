#include "intervalplot.h"
#include "recordedplot.h"
#include "gui/gui_funcs.h"
#include "global_define.h"
#include "markitem.cpp"

extern QFile mRawDataFile;
extern QFile mMarksFile;

QPair<double, double> mFirstIntervalMinMaxXRange = qMakePair(0, 60);
QPair<double, double> mSecondIntervalMinMaxXRange = qMakePair(0, 60);

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

#define TIME_INTERVAL_FOR_RECORD_IN_FILE (4)
#define TIME_INTERVAL_FOR_WRITE_ON_GRAPH (40) //40 миллисекунд - 25 раз в секунду
#define TIME_INTERVAL_DIFF (TIME_INTERVAL_FOR_WRITE_ON_GRAPH / TIME_INTERVAL_FOR_RECORD_IN_FILE)

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

//    for (uint16_t i=0; i<dataVector.count(); i++)
//    {
//        mMainGraph->addData(dataVector[i].first, dataVector[i].second);
//    }
//    xAxis->setRange(dataVector[0].first, dataVector[dataVector.count()-1].first);
    qDebug() << "start" << points.first << "stop" << points.second;
    uint iterTemp = 0;
    iterTemp = ((points.second-points.first)*TIME_INTERVAL_DIFF)+1;
    _mSPIData tempArr[iterTemp];
    mRawDataFile.open(QIODevice::ReadOnly);
    mRawDataFile.seek(points.first*TIME_INTERVAL_DIFF*sizeof(_mSPIData));
    mRawDataFile.read((char*)&tempArr, sizeof(tempArr));
    mRawDataFile.close();
    for (uint i=0; i<iterTemp; i++)
    {
        mMainGraph->addData((double)tempArr[i].timeStamp/1000, tempArr[i].data);
    }
    QString ttt;
    QStringList list;

    mMarksFile.open(QIODevice::ReadOnly);
    if (mMarksFile.isOpen())
    {
        ttt = mMarksFile.readAll();
        mMarksFile.seek(0);
        mMarksFile.close();
    }
    else
    {
        qDebug() << "not open";
    }
    list = ttt.split("\n");
    MarkItem *newMark = nullptr;
    QFont mFontForLabelItems {"Sans Serif", 16};
    for (int i=0; i<list.count()-1; i++)
    {
        double currPos = list[i].split(": ")[1].toDouble()/1000;
        if (currPos > mMainGraph->data()->at(0)->key && currPos < mMainGraph->data()->at(iterTemp-1)->key)
        {
            newMark = new MarkItem(this, list[i].split(": ")[0], currPos, mFontForLabelItems);
        }
    }
    qDebug() << list;
    if (mCurrentIntervalNum == 1)
    {
        mFirstIntervalMinMaxXRange = qMakePair((double)tempArr[0].timeStamp/1000, (double)tempArr[(points.second-points.first)*TIME_INTERVAL_DIFF].timeStamp/1000);
    }
    else
    {
        mSecondIntervalMinMaxXRange = qMakePair((double)tempArr[0].timeStamp/1000, (double)tempArr[(points.second-points.first)*TIME_INTERVAL_DIFF].timeStamp/1000);
    }
    xAxis->setRange((double)tempArr[0].timeStamp/1000, (double)tempArr[(points.second-points.first)*TIME_INTERVAL_DIFF].timeStamp/1000);
}


void IntervalPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("секунды"));
    mICPSettings->getCurrentPressureIndex() == 0 ? yAxis->setLabel(tr("мм рт ст")) : yAxis->setLabel(tr("мм вод ст"));

}

void IntervalPlot::scaleFont(double scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
}

bool IntervalPlot::event(QEvent *event)
{
    // сенсор
    return AbstractCustomPlot::event(event);
}
