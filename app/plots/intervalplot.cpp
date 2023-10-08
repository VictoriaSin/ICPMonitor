#include "intervalplot.h"
#include "recordedplot.h"
#include "gui/gui_funcs.h"

#include "markitem.cpp"
extern QFile mTestData;
extern QFile mRawDataFile;
extern QFile mMarksFile;

QPair<float, float> mFirstIntervalMinMaxXRange = qMakePair(0, 60);
QPair<float, float> mSecondIntervalMinMaxXRange = qMakePair(0, 60);

IntervalPlot::IntervalPlot(QWidget *parent):
    AbstractCustomPlot(GraphType::IntervalGraph, parent)
{
    mMainGraph = addGraph();
    mAvgGraph = addGraph();
    average = addGraph();
    substract = addGraph();
    amplitude = addGraph();

    mMainGraph->setAntialiased(true);
    addLayer("lineLayer", nullptr, limAbove);
    layer("lineLayer")->setMode(QCPLayer::lmBuffered);
    mICPSettings->getCurrentPressureIndex() == 0 ? mCurrentMaxYRange = 60 : mCurrentMaxYRange = 815.7;
    setInteraction(QCP::iRangeDrag, true); // Можем передвигать график
    setInteraction(QCP::iRangeZoom, true); // Можем приближать график
    firstBuffPointer  = 0;
    lastBuffPointer   = 0;
    sum               = 0;
    cnt               = 0;

    retranslate();    
}

IntervalPlot::~IntervalPlot()
{
    delete[] CurrDataForAverage;
}



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
    pen.setColor(Qt::GlobalColor::gray);
    //mAvgGraph->setPen(pen);
    //mAvgGraph->setBrush(brush);

    average->setPen(pen);
    average->setBrush(brush);
    pen.setColor(Qt::GlobalColor::green);
    substract->setPen(pen);
    substract->setBrush(brush);
    pen.setColor(Qt::GlobalColor::lightGray);
    amplitude->setPen(pen);
    amplitude->setBrush(brush);

//500 50
    if (isDownloadGraph)
    {
        points.first= (points.first - (points.first % 2)) * 0.3;
        points.second = (points.second  - (points.second % 2)) * 0.3 ;
        maxBuffSizeAvg =(uint) (1000.0 / 20 * 0.6);
    }
    else
    {
        points.first= (points.first - (points.first % 2)) * 3;
        points.second = (points.second  - (points.second % 2)) * 3 ;
        maxBuffSizeAvg =(uint) (1000.0 / TIME_INTERVAL_FOR_WRITE_ON_GRAPH * 0.6);
    }
    CurrDataForAverage = new uint16_t[maxBuffSizeAvg];
    qDebug() << "start1" << points.first << "stop2" << points.second;

    iterTemp = ((points.second-points.first)/6) + 1;

    _mSPIData *tempArr = new _mSPIData[iterTemp];
    QFile *currFile;
    if (isDownloadGraph)
    {
        currFile = &mTestData;
    }
    else
    {
        currFile = &mRawDataFile;
    }
    currFile->open(QIODevice::ReadOnly);
    currFile->seek(points.first);
    currFile->read((char*)tempArr, iterTemp*6);
    currFile->close();
    float param = 1.0;
    if (mICPSettings->getCurrentPressureIndex() == 1)
    {
        param = indexPressureH2O;
    }
    for (uint i=0; i<iterTemp; i++)
    {
        mMainGraph->addData((float)tempArr[i].timeStamp/1000, tempArr[i].data*param);
        //averagePlot(tempArr[i]);
    }
    Compliance();
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


void IntervalPlot::averagePlot(_mSPIData temp)
{
    firstBuffPointer = (++firstBuffPointer) % maxBuffSizeAvg;
    sum += temp.data;
    if (cnt < maxBuffSizeAvg) { cnt++; }
    else
    {
        lastBuffPointer = (++lastBuffPointer) % maxBuffSizeAvg;
        sum -= CurrDataForAverage[lastBuffPointer];
    }
    CurrDataForAverage[firstBuffPointer] = temp.data;
    mAvgGraph->addData((float)temp.timeStamp/1000, (float)sum/cnt);
}

void IntervalPlot::Compliance()
{
    uint16_t widthWin = 30;
    uint32_t N = mMainGraph->data()->size();
    // скользящее среднее
    if(N-widthWin>0)
    {
        double mid=0.0;
        //    QVector<double> win(widthWin);
        QVector<double> xf(N - widthWin), yf(N-widthWin);
        for (uint16_t i = widthWin; i < N; i++)
        {
            mid=0.0;
            for(int j=0;j<widthWin;j++)
            {
                //        win[j]=yData[i-widthWin+j];
                //mid += yData[i - widthWin+j];
                mid += mMainGraph->data()->at(i - widthWin+j)->value;
            }
            yf[i-widthWin]=mid/widthWin;
            //xf[i-widthWin]=xData[i];
            xf[i-widthWin]=mMainGraph->data()->at(i)->key;
        }
        //ui->customPlot->addGraph();

        average->addData(xf, yf);


        QVector<double> yd(N-widthWin); // вычитание графиков

        for (uint16_t i = widthWin; i < N; i++)
        {
            //yd[i-widthWin]=yData[i] - yf[i-widthWin];
            yd[i-widthWin]=mMainGraph->data()->at(i)->value - yf[i-widthWin];
        }
        //ui->customPlot->addGraph();
        //ui->customPlot->graph(2)->setData(xf, yd);

        substract->addData(xf, yd);

        QVector<double> xa,ya; // амплитудный график
        bool bp=false;
        double ydiff, xmax=xf[0], ymax=yd[0];
        for(uint16_t i = widthWin; i < N; i++)
        {
            ydiff=yd[i-widthWin];
            if(ydiff>0){
                if(ydiff>ymax){
                    ymax=ydiff;  xmax=xf[i-widthWin];
                }
                bp=true;
            }
            else
            {
                if(bp)
                {
                    xa.append(xmax);
                    ya.append(ymax);
                    ymax=ydiff;  xmax=xf[i-widthWin];
                }
                bp=false;
            }
        }
        //    xaData=xa;yaData=ya;
        // убираю неправильные амплитуды
        double ya_mid=0.0;
        QVector<double> xaData,yaData; // амплитудный график
        xaData.clear();
        yaData.clear();
        int na=ya.size();
        if(na==0) goto fin;
        for(int i=0;i<na;i++)
        {
            ya_mid+=ya[i];
        }
        ya_mid/=na;
        for(int i=0;i<na;i++)
        {
            if(ya[i] > 0.1*ya_mid)
            {
                xaData.append(xa[i]);
                yaData.append(ya[i]);
            }
        }
fin:
        //ui->customPlot->addGraph();
        //ui->customPlot->graph(3)->setData(xaData, yaData);

        amplitude->addData(xaData, yaData);
    }
}
