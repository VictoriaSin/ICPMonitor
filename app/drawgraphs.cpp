#include "drawgraphs.h"
#include <QDebug>
#include "../app/global_define.h"
#include "../app/plots/waveformplot.h"
//#include "../core/sensor/zsc.h"
#include "../core/sensor/spiThread.h"

//class ZSC; ZSC mZSC1;

class spiThread;
extern spiThread *mSpiThread;

class WaveFormPlot;
extern  WaveFormPlot *mWaveGraph;
extern  WaveFormPlot *mComplianceGraph;
extern float dVConst;
extern QFile mRawDataSessionRecordFile;
DrawGraphs::DrawGraphs() : QThread() {}
DrawGraphs::~DrawGraphs(){}
void DrawGraphs::run()
{
  _mSPIData temp;
  isStopped         = false;
  isRunning         = true;
  temp.data         = 0;
  temp.timeStamp    = 0;
  sum               = 0;
  currIndex         = 0;
  pointTime         = 0;
  uint16_t axisXRange    = (uint16_t)(mWaveGraph->xAxis->range().size() * 1000);
  float max = 0;
  u32 max_pos = 0;
  bool needDraw = false;

#define DRAW_TIME 2
#define GRAPH_SIZE_AVG ((uint32_t)(mWaveGraph->xAxis->range().size() * 25))

  mData_2 = 0;
  float k = 0.05;
  globalCount = 0;
  float filVal;
  float data1 = 0;
  float data2 = 0;
  float param = 1.0;
  if (mICPSettings->getCurrentPressureIndex() == 1)
  {
    param = indexPressureH2O;
  }
  mWaveGraph->graphRangeSize = mWaveGraph->xAxis->range().size()*1000;
  mWaveGraph->graphCurrentMaxRange = mWaveGraph->graphRangeSize;
  mWaveGraph->graphMinus = 0;
  mWaveGraph->mMainGraph->data().clear();
  mWaveGraph->mHistGraph->data().clear();

  mComplianceGraph->mMainGraph->data().clear();
  mComplianceGraph->mHistGraph->data().clear();
  mComplianceGraph->graphRangeSize = mComplianceGraph->xAxis->range().size()*1000;
  mComplianceGraph->graphCurrentMaxRange= mComplianceGraph->graphRangeSize;
  mComplianceGraph->graphMinus = 0;
  volatile qint64 startTime = getCurrentTimeStamp_ms();
  volatile qint64 stopTimeGraph = startTime + 2;//TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
  volatile qint64 currentTime;
  qint64 offsetTime = 0;
  float compliance;
  qDebug() << "DrawGraphs started";

  if (mSaveInFile == nullptr)  { mSaveInFile = new SaveSPI();}
  mSaveInFile->start(QThread :: HighestPriority);

  filVal = (float)mSpiThread->rawData * param/1000;
  while(isRunning)
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime < stopTimeGraph) {continue;}

    stopTimeGraph += DRAW_TIME;
    temp.timeStamp = (uint32_t)(currentTime - startTime);
    if (currIndex == 0) { pointTime = temp.timeStamp; }
    currIndex++;
    sum += mSpiThread->rawData;

    if (currIndex == 10)
    {
      currIndex = 0;
      //data1 = (float)sum * param / 10000;
      data1 = (float)mSpiThread->rawData * param / 1000;
      sum = 0;
      mWaveGraph->addDataOnGraphic(pointTime, data1);
      filVal += ((data1 - filVal) * k);
      //mWaveGraph->mTempGraph->addData((float)(pointTime - (uint32_t)offsetTime) / 1000, filVal);
      data2 = filVal;
      if (data1 > data2)
      {
        if (data1 > max)
        {
          max       = data1;
          max_pos   = pointTime;
          needDraw  = true;
        }
      }
      else
      {
        if (needDraw)
        {
            mWaveGraph->mAmplitudePoints->addData((float)(max_pos - (uint32_t)offsetTime) / 1000, max);
            //qDebug() << "globalCount" << globalCount << max_pos << max;
            compliance = dVConst/(2*(max - data2));
            mComplianceGraph->addDataOnGraphic(max_pos, compliance);
            qDebug() << "comp" << compliance << "dVConst" << dVConst;
            needDraw = false;
            max = 0;
            max_pos = 0;
        }
      }

      if ((temp.timeStamp - offsetTime) >= axisXRange)
      {
        offsetTime = temp.timeStamp;
        mWaveGraph->mAmplitudePoints->data()->clear();
      }
    }

    //qDebug() << stopTimeGraph << temp.timeStamp ;
  }
  isStopped = true;
  mSaveInFile->isRunning = false;
  while(mSaveInFile->isStopped == false);
  qDebug() << "DrawGraphs stopped";
}


