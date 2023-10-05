#include "savespi_1.h"
#include <QDebug>
#include "../app/plots/waveformplot.h"
#include "../core/sensor/zsc.h"

class ZSC;
extern ZSC mZSC;
class WaveFormPlot;
extern  WaveFormPlot *mWaveGraph;
SaveSPI_1::SaveSPI_1() : QThread()
{
}

SaveSPI_1::~SaveSPI_1()
{

}


void SaveSPI_1::run()
{

  isStopped   = false;
  isRunning   = true;
  isRecording = false;
  temp.data = 0;
  temp.timeStamp = 0;

  qDebug() << "SaveSPI_1 started";
  volatile qint64 startTime = getCurrentTimeStamp_ms();
  volatile qint64 stopTimeFile = startTime + TIME_INTERVAL_FOR_RECORD_IN_FILE;
  volatile qint64 currentTime;

  while (isRunning)
  {
    currentTime = getCurrentTimeStamp_ms();

    if (currentTime > stopTimeFile)
    {
      stopTimeFile += TIME_INTERVAL_FOR_RECORD_IN_FILE;
      READ_SPI_DATA();
      temp.data = mZSC.data[0];
      temp.timeStamp = (uint32_t)(currentTime - startTime);
      //qDebug() << "temp.timeStamp" << temp.timeStamp;
      mRawDataFile.write((char*)&temp, sizeof(_mSPIData));
      //QThread::msleep(1);
      mZSC.oneShot();
    }
    else QThread::usleep(50); //400
  }
  qDebug() << "SaveSPI_1 stopped";
  isStopped = true;
}
