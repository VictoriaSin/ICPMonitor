#include "savespi.h"
#include <QDebug>
//#include "../app/plots/abstractmultiplegraphsareaswidget.h"
//#include "../app/plots/currentgraphsarea.h"
#include "../app/global_define.h"
#include "../app/plots/waveformplot.h"


//extern uint8_t currBuffer;
class WaveFormPlot;
extern  WaveFormPlot *mWaveGraph;
SaveSPI::SaveSPI() : QThread() //QObject *parent)
/*, QString dirPath)*/ //{parent}
{
    //SPIData.setFileName(dirPath + "/SPI_Data.txt");
}

SaveSPI::~SaveSPI()
{

}

#define READ_SPI_DATA() temp.data = (12 + rand() % 20)
void SaveSPI::run()
{
  _mSPIData temp;
    isStopped   = false;
    isRunning   = true;
    isRecording = false;
    temp.data = 0;
    temp.timeStamp = 0;

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
      qDebug() << "temp.timeStamp" << temp.timeStamp;
      mWaveGraph->addDataOnGraphic(temp.timeStamp, temp.data);
    }

    temp.data = 0;
    temp.timeStamp = 0;

    qDebug() << "SaveSPI start record";
    startTime = getCurrentTimeStamp_ms();
    stopTimeGraph = startTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
    volatile qint64 stopTimeFile = startTime + TIME_INTERVAL_FOR_RECORD_IN_FILE;
    while (isRunning)
    {
      currentTime = getCurrentTimeStamp_ms();

      if (currentTime > stopTimeFile)
      {
        stopTimeFile = currentTime + TIME_INTERVAL_FOR_RECORD_IN_FILE;
        READ_SPI_DATA();
        temp.timeStamp = (uint32_t)(currentTime - startTime);
        mRawDataFile.write((char*)&temp, sizeof(_mSPIData));
      }

      if (currentTime > stopTimeGraph)
      {
        stopTimeGraph = currentTime + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
        qDebug() << "temp.timeStamp" << temp.timeStamp;
        mWaveGraph->addDataOnGraphic(temp.timeStamp, temp.data);
      }
    }
    qDebug() << "SaveSPI stopped";
    isStopped = true;
    //fillFile();

}

//void SaveSPI::fillFile()
//{
//    SPIData.open(QIODevice::WriteOnly | QIODevice::Append);
//    if (currBuffer == BUFFER_2)
//    {
//        qDebug() << "spi write buf1";
//        //SPIData.write((char*)&mSensorBuffer1.record, mSensorBuffer1.index * sizeof(_mSPIData));
//        //mSensorBuffer1.index = 0;
//    }
//    else
//    {
//        qDebug() << "spi write buf2";
//        //SPIData.write((char*)&mSensorBuffer2.record, mSensorBuffer2.index * sizeof(_mSPIData));
//        //mSensorBuffer2.index = 0;
//    }
//    SPIData.close();
//}
