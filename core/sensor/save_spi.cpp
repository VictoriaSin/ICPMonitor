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
SaveSPI::SaveSPI(/*QFile file*/) : QThread()
{
    /*fileForSave = new QFile(file.fileName());
    qDebug() << "file name" << fileForSave.fileName();*/
}
SaveSPI::~SaveSPI(){}
void SaveSPI::run()
{
  qDebug("SaveSPI Thread Started");
  isStopped   = false;
  isRunning   = true;
  isRecording = false;
  currMesuring.data = 0;
  currMesuring.timeStamp = 0;
  mSpiThread = new spiThread(mZSC.mFd);
  mSpiThread->start();
  /////////////////////

//  if (fileForSave.open(QIODevice::WriteOnly | QIODevice::Append))
//  {qDebug() << "opened";}
//  else
//  {
//      qDebug() << "not opened";
//  }
//  if (fileForSave.exists()) qDebug() << "exists";
  /////////////////////
  volatile qint64 currentTime;
  startTime = getCurrentTimeStamp_ms();
  volatile qint64 stopTimeFile = startTime + TIME_INTERVAL_FOR_RECORD_IN_FILE;
  while (isRunning)
  {
    currentTime = getCurrentTimeStamp_ms();
    if (currentTime > stopTimeFile)
    {
      stopTimeFile += TIME_INTERVAL_FOR_RECORD_IN_FILE;
      currMesuring.data = mSpiThread->rawData; //mZSC.data[0];
      currMesuring.timeStamp = (uint32_t)(currentTime - startTime);
      if (isRecording) { /*fileForSave*/mRawDataFile.write((char*)&currMesuring, sizeof(_mSPIData));}//
    }
    else QThread::usleep(50); //400
  }
  //////////////////////
//  fileForSave.close();
  //////////////////////
  mSpiThread->isRunning = false;
  while(!mSpiThread->isStopped);
  qDebug("SaveSPI Thread Stopped");
  isStopped = true;
}
