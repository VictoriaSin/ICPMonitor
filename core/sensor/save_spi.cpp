#include "save_spi.h"
#include <QDebug>
#include "../app/plots/waveformplot.h"
#include "../core/sensor/zsc.h"
#include "../core/sensor/spiThread.h"
class spiThread;
class ZSC;
extern ZSC mZSC;

class WaveFormPlot;
extern  WaveFormPlot *mWaveGraph;
SaveSPI::SaveSPI() : QThread(){}
SaveSPI::~SaveSPI(){}
void SaveSPI::run()
{
  qDebug("SaveSPI Thread Started");
  isStopped   = false;
  isRunning   = true;
  isRecording = false;
  temp.data = 0;
  temp.timeStamp = 0;

  spiThread *mSpiThread = new spiThread(mZSC.mFd);
  mSpiThread->start();
  startTime = getCurrentTimeStamp_ms();
  volatile qint64 stopTimeFile = startTime + TIME_INTERVAL_FOR_RECORD_IN_FILE;
  volatile qint64 currentTime;

  while (isRunning)
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime > stopTimeFile)
    {
      stopTimeFile += TIME_INTERVAL_FOR_RECORD_IN_FILE;
      temp.data = mSpiThread->rawData; //mZSC.data[0];
      temp.timeStamp = (uint32_t)(currentTime - startTime);
      if (isRecording) { mRawDataFile.write((char*)&temp, sizeof(_mSPIData)); }
      //mZSC.spi_oneShot();
    }
    else QThread::usleep(50); //400
  }
  mSpiThread->isRunning = false;  while(!mSpiThread->isStopped);
  qDebug("SaveSPI Thread Stopped");
  isStopped = true;
}
