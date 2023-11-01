#include "read_spi.h"
#include <QDebug>
#include "../app/global_define.h"
#include "../app/plots/waveformplot.h"

//#include "../app/gui/mainwindow.h"
#include "../app/gui/mainpage.h"
#include "../core/sensor/zsc.h"


class ZSC;
ZSC mZSC;

class WaveFormPlot;
class MainPage;
//class MainWindow;
extern  WaveFormPlot *mWaveGraph;
extern MainPage *mMainPage;
ReadSPI::ReadSPI() : QThread() {}
ReadSPI::~ReadSPI(){}
void ReadSPI::run()
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
  float currData;

  if (mSaveSPI == nullptr)  { mSaveSPI = new SaveSPI();  }
  mSaveSPI->start(QThread :: HighestPriority);

  while(isRunning && (!isRecording))
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime < stopTimeGraph) continue;
    stopTimeGraph = currentTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
    temp.timeStamp = (uint32_t)(currentTime - startTime);
    currData = (float)mSaveSPI->currMesuring.data*param/1000;   //mZSC.data[0]*param/1000;
    mMainPage->setAverage(calcAverage(currData));
    mWaveGraph->addDataOnGraphic(temp.timeStamp, currData);
  }

  if (isRunning)
  {
    qDebug("Recording data start");
    mWaveGraph->mMainGraph->data()->clear();
    mWaveGraph->mHistGraph->data()->clear();
    temp.data = 0;
    temp.timeStamp = 0;
    mWaveGraph->graphCurrentMaxRange = mWaveGraph->graphRangeSize;
    mWaveGraph->graphMinus = 0;
    mSaveSPI->isRecording = true;
    startTime = getCurrentTimeStamp_ms();
    mSaveSPI->startTime = startTime;
    stopTimeGraph = startTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
    while (isRunning)
    {
      currentTime = getCurrentTimeStamp_ms();
      if (currentTime > stopTimeGraph)
      {
        stopTimeGraph += TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
        currData = (float)mSaveSPI->currMesuring.data*param/1000;
        mMainPage->setAverage(calcAverage(currData));
        mWaveGraph->addDataOnGraphic(mSaveSPI->currMesuring.timeStamp, currData);
        QThread::msleep(5);
      }
      else
      {
        QThread::msleep(1);
      }
    }
    if (CurrDataForAverage != nullptr) delete [] CurrDataForAverage;
  }
  mSaveSPI->isRunning = false;
  while(mSaveSPI->isStopped == false);
  qDebug("ReadSPI Threads Stopped");
  isStopped = true;
}

uint16_t ReadSPI::calcAverage(uint16_t data)
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
