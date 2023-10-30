#include "drawgraphs.h"
#include <QDebug>
#include "../app/global_define.h"
#include "../app/plots/waveformplot.h"
#include "../core/sensor/zsc.h"

class ZSC;
ZSC mZSC1;


class WaveFormPlot;
//class MainPage;
extern  WaveFormPlot *mWaveGraph;
extern  WaveFormPlot *mComplianceGraph;
extern double dVConst;
//extern MainPage *mMainPage;


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

  float max = 0;
  float max_pos = 0;
  bool needDraw = false;
  bool up = false;

#define CNT_DATA 21
  //mData_1 = new uint16_t[CNT_DATA];
  mData_2 = 0;//new uint16_t[CNT_DATA];
  float k = 0.05;//ui->mFilterK->text().toFloat();
  globalCount = 0;
  float filVal;
  float data1 = 0;
  float data2 = 0;
  float pos_data1 = 0;

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

  mZSC1.spi_oneShot();
  filVal = (float)mZSC1.data[0]*param/1000;

#define DRAW_TIME 2
#define GRAPH_SIZE_AVG ((uint32_t)(mWaveGraph->xAxis->range().size() * 25))
  while(isRunning)
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime < stopTimeGraph) {continue;}

    stopTimeGraph +=2;//= currentTime + 2;//DRAW_TIME;
    temp.timeStamp = (uint32_t)(currentTime - startTime);
    if (currIndex == 0) { pointTime = temp.timeStamp; }
    currIndex++;
    sum += mZSC1.data[0];

    if (currIndex == 10)
    {
      currIndex = 0;
      data1 = (float)sum * param / 10000;
      sum = 0;
      mWaveGraph->addDataOnGraphic(pointTime, data1);
      filVal += (((float)mZSC1.data[0] * param / 1000 - filVal) * k);
      data2 = filVal;
      pos_data1 = (float)(temp.timeStamp - offsetTime)/1000;
      //mWaveGraph->mTempGraph->addData(pos_data1, filVal);
      if (data1 > data2)
      {
        if (data1 > max)
        {
          max       = data1;
          max_pos   = pos_data1;
          needDraw  = true;
        }
      }
      else
      {
        if (needDraw)
        {
          uint32_t ttt = max_pos * 1000;
          mWaveGraph->mAmplitudePoints->addData(max_pos, max);
          qDebug() << "globalCount" << globalCount << max_pos << max;
          //qDebug() << ">0" << max_pos << max;
          compliance = dVConst/(2*(max - data2));
          if (ttt > mComplianceGraph->graphCurrentMaxRange)
          {
            *mComplianceGraph->mHistGraph->data() = *mComplianceGraph->mMainGraph->data();
            //mComplianceGraph->mMainGraph->data().data()->clear();
            mComplianceGraph->graphCurrentMaxRange += mComplianceGraph->graphRangeSize;
            mComplianceGraph->graphMinus += mComplianceGraph->graphRangeSize;
          }
          ttt-= mComplianceGraph->graphMinus;
          float temp_x = (float) ttt/1000;
          if(mComplianceGraph->mHistGraph->data()->size())
          {
            mComplianceGraph->mHistGraph->data()->removeBefore(temp_x + 0.5);
          }
          mComplianceGraph->mMainGraph->addData(max_pos, compliance);

          needDraw = false;
          max = 0;
          max_pos = 0;
        }
      }


      //globalCount++;
      //globalCount %= GRAPH_SIZE_AVG;

      //if (globalCount == 0)
      if ((temp.timeStamp - offsetTime) >= 12000)
      {
        offsetTime = temp.timeStamp;
        mWaveGraph->mAmplitudePoints->data()->clear();
        mComplianceGraph->mMainGraph->data()->clear();
        //globalCount = 0;
      }
    }
    mZSC1.spi_oneShot();
    //qDebug() << stopTimeGraph << temp.timeStamp ;
  }
  isStopped = true;
  qDebug() << "DrawGraphs stopped";
}
