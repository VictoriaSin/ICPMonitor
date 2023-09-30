#include "savespi.h"
#include <QDebug>
#include "../app/global_define.h"
#include "../app/plots/waveformplot.h"

class WaveFormPlot;
extern  WaveFormPlot *mWaveGraph;
SaveSPI::SaveSPI() : QThread()
{
}

SaveSPI::~SaveSPI()
{

}

//#define READ_SPI_DATA() temp.data = (12 + rand() % 20)

void SaveSPI::run()
{
  _mSPIData temp;
  isStopped   = false;
  isRunning   = true;
  isRecording = false;
  temp.data = 0;
  temp.timeStamp = 0;

  mWaveGraph->graphRangeSize = mWaveGraph->xAxis->range().size()*1000;
  mWaveGraph->graphCurrentMaxRange = mWaveGraph->graphRangeSize;
  mWaveGraph->graphMinus = 0;
  mWaveGraph->mMainGraph->data().clear();
  mWaveGraph->mHistGraph->data().clear();
  qDebug() << "SaveSPI started";
  volatile qint64 startTime = getCurrentTimeStamp_ms();
  volatile qint64 stopTimeGraph = startTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
  volatile qint64 currentTime;

  while(isRunning && (!isRecording))
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime < stopTimeGraph) continue;
    stopTimeGraph = currentTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
    READ_SPI_DATA();
    temp.timeStamp = (uint32_t)(currentTime - startTime);
    //qDebug() << "temp.timeStamp" << temp.timeStamp;
    mWaveGraph->addDataOnGraphic(temp.timeStamp, temp.data);
  }

  if (isRunning)
  {
    temp.data = 0;
    temp.timeStamp = 0;
    mWaveGraph->graphCurrentMaxRange = mWaveGraph->graphRangeSize;
    mWaveGraph->graphMinus = 0;
    mWaveGraph->mMainGraph->data().clear();
    mWaveGraph->mHistGraph->data().clear();
    if (mSaveSPI_1 == nullptr)
    {
      mSaveSPI_1 = new SaveSPI_1();
    }
    mSaveSPI_1->start(QThread :: HighestPriority);

    qDebug() << "SaveSPI start record";
    startTime = getCurrentTimeStamp_ms();
    stopTimeGraph = startTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
    while (isRunning)
    {
      currentTime = getCurrentTimeStamp_ms();
      if (currentTime > stopTimeGraph)
      {
        stopTimeGraph += TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
        mWaveGraph->addDataOnGraphic(mSaveSPI_1->temp.timeStamp, mSaveSPI_1->temp.data);
        QThread::msleep(5);
      }
      else
      {
        QThread::msleep(1);
      }
    }

    mSaveSPI_1->isRunning = false;
    while(mSaveSPI_1->isStopped == false);
  }
  qDebug() << "SaveSPI stopped";
  isStopped = true;
}
