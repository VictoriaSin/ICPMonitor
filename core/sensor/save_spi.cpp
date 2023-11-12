#include "save_spi.h"
#include <QDebug>
#include "../app/plots/waveformplot.h"
#include "../core/sensor/zsc.h"
#include "../core/sensor/spiThread.h"
class spiThread;
class ZSC;
extern ZSC mZSC;
spiThread *mSpiThread;

class WaveFormPlot;
extern  WaveFormPlot *mWaveGraph;
extern QString globalFolder;

SaveSPI::SaveSPI() : QThread(){}
SaveSPI::~SaveSPI(){}
void SaveSPI::run()
{
  qDebug("\033[36m>>mSaveSPI Thread Started\033[0m");
  isStopped   = false;
  isRunning   = true;
  isRecording = false;
  currMesuring.data = 0;
  currMesuring.timeStamp = 0;
  mSpiThread = new spiThread(mZSC.mFd);
  mSpiThread->start();
  /////////////////////

  //if (fileForSave->open(QIODevice::WriteOnly))
  //{qDebug() << "opened";}
  //else   {qDebug() << "not opened";}
  //if (fileForSave->exists()) qDebug() << "exists";
  /////////////////////
  volatile qint64 currentTime;
  startTime = getCurrentTimeStamp_ms();
  volatile qint64 stopTimeFile = startTime + TIME_INTERVAL_FOR_RECORD_IN_FILE;
  while (isRunning && (!isRecording))
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime > stopTimeFile)
    {
      stopTimeFile += TIME_INTERVAL_FOR_RECORD_IN_FILE;
      currMesuring.data = mSpiThread->rawData;
      currMesuring.timeStamp = (uint32_t)(currentTime - startTime);      
    }
    else QThread::usleep(50); //400
  }

  if (isRunning)
  {
      ////////
      fileForSave = new QFile(fileName);
      fileForSave->open(QIODevice::WriteOnly);
      ////////
      while (isRunning)
      {
          currentTime = getCurrentTimeStamp_ms();
          if (currentTime > stopTimeFile)
          {
            stopTimeFile += TIME_INTERVAL_FOR_RECORD_IN_FILE;
            currMesuring.data = mSpiThread->rawData;
            currMesuring.timeStamp = (uint32_t)(currentTime - startTime);
            fileForSave->write((char*)&currMesuring, sizeof(_mSPIData));
          }
          else QThread::usleep(50); //400
      }
      //////////////////////
      fileForSave->close();
      delete fileForSave;
      //////////////////////
  }

  mSpiThread->isRunning = false;
  while(!mSpiThread->isStopped);
  qDebug("\033[36m<<SaveSPI Thread Stopped\033[0m");
  isStopped = true;
}
