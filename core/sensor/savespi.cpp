#include "savespi.h"
#include <QDebug>
#include "../app/global_define.h"
#include "../app/plots/waveformplot.h"

//#include "../app/gui/mainwindow.h"
#include "../app/gui/mainpage.h"
#include "../core/sensor/zsc.h"

#ifndef PC_BUILD
class ZSC;
ZSC mZSC;
#endif

class WaveFormPlot;
class MainPage;
//class MainWindow;
extern  WaveFormPlot *mWaveGraph;
extern MainPage *mMainPage;
SaveSPI::SaveSPI() : QThread()
{
}

SaveSPI::~SaveSPI()
{

}

void SaveSPI::run()
{
  _mSPIData temp;
  isStopped         = false;
  isRunning         = true;
  isRecording       = false;
  temp.data         = 0;
  temp.timeStamp    = 0;
  firstBuffPointer  = 0;
  lastBuffPointer   = 0;
  sum               = 0;
  cnt               = 0;

  float param = 1.0;
  if (mICPSettings->getCurrentPressureIndex() == 1)
  {
      param = indexPressureH2O;
  }
  maxBuffSizeAvg =(uint) (1000.0 / TIME_INTERVAL_FOR_WRITE_ON_GRAPH * AverageIntervalSec);
  CurrDataForAverage = new uint16_t[maxBuffSizeAvg];

  mWaveGraph->graphRangeSize = mWaveGraph->xAxis->range().size()*1000;
  mWaveGraph->graphCurrentMaxRange = mWaveGraph->graphRangeSize;
  mWaveGraph->graphMinus = 0;
  mWaveGraph->mMainGraph->data().clear();
  mWaveGraph->mHistGraph->data().clear();

  volatile qint64 startTime = getCurrentTimeStamp_ms();
  volatile qint64 stopTimeGraph = startTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
  volatile qint64 currentTime;

  qDebug() << "SaveSPI started";
  while(isRunning && (!isRecording))
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime < stopTimeGraph) continue;
    stopTimeGraph = currentTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
#ifdef PC_BUILD
    READ_SPI_DATA();
#else
  #ifdef FOR_TEST_ONLY
    READ_SPI_DATA();
#else
    temp.data = mZSC.data[0];
#endif
#endif
    temp.timeStamp = (uint32_t)(currentTime - startTime);
    mMainPage->setAverage(calcAverage(temp.data*param));
    mWaveGraph->addDataOnGraphic(temp.timeStamp, temp.data*param);
#ifndef PC_BUILD
    mZSC.oneShot();
#endif
  }

  if (isRunning)
  {
    mWaveGraph->mMainGraph->data()->clear();
    mWaveGraph->mHistGraph->data()->clear();
    if (mSaveSPI_1 == nullptr)
    {
      mSaveSPI_1 = new SaveSPI_1();
    }
    mSaveSPI_1->start(QThread :: HighestPriority);
    temp.data = 0;
    temp.timeStamp = 0;
    mWaveGraph->graphCurrentMaxRange = mWaveGraph->graphRangeSize;
    mWaveGraph->graphMinus = 0;
    //qDebug() << "SaveSPI start record";
    startTime = getCurrentTimeStamp_ms();
    stopTimeGraph = startTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
    while (isRunning)
    {
      currentTime = getCurrentTimeStamp_ms();
      if (currentTime > stopTimeGraph)
      {
        stopTimeGraph += TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
        mMainPage->setAverage(calcAverage(mSaveSPI_1->temp.data*param));
        mWaveGraph->addDataOnGraphic(mSaveSPI_1->temp.timeStamp, mSaveSPI_1->temp.data*param);
        QThread::msleep(5);
      }
      else
      {
        QThread::msleep(1);
      }
    }

    if (CurrDataForAverage != nullptr) delete [] CurrDataForAverage;
    mSaveSPI_1->isRunning = false;
    while(mSaveSPI_1->isStopped == false);
  }
  //qDebug() << "SaveSPI stopped";
  isStopped = true;
}
uint16_t SaveSPI::calcAverage(uint16_t data)
{
  firstBuffPointer = (++firstBuffPointer) % maxBuffSizeAvg;
  sum += data;
  if (cnt < maxBuffSizeAvg) { cnt++; }
  else
  {
    lastBuffPointer = (++lastBuffPointer) % maxBuffSizeAvg;
    sum -= CurrDataForAverage[lastBuffPointer];
  }
  CurrDataForAverage[firstBuffPointer] = data;
  return sum/cnt;
}
