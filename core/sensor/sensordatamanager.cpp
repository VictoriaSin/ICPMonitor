#include "sensordatamanager.h"
#include "impl/fileimitsensor.h"
#include "../app/global_define.h"

#include "stdlib.h"

#include <QVector>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include "math.h"


#define selectCurrentBufferForRecord(_BUF) { currBuffer = _BUF;}
uint8_t currBuffer = BUFFER_1;
//_mSPIBuffer mSensorBuffer1, mSensorBuffer2;

#define TIME_INTERVAL_FOR_RECORD_IN_FILE (10)
#define TIME_INTERVAL_FOR_WRITE_ON_GRAPH (40) //40 миллисекунд - 25 раз в секунду

#define initTimerGraph() {timerForSetInGraph = getCurrentTimeStamp_ms() + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;}
#define initTimerRecord() {timerForRecordInFile =  getCurrentTimeStamp_ms() + TIME_INTERVAL_FOR_RECORD_IN_FILE;}
#define startTimerGraph() {timerForSetInGraph += TIME_INTERVAL_FOR_WRITE_ON_GRAPH;}
#define startTimerRecord() {timerForRecordInFile += TIME_INTERVAL_FOR_RECORD_IN_FILE;}

#define READ_SPI() data = (12 + rand() % 20) // Потом будем брать значение из потока SPI

SensorDataManager::SensorDataManager(QObject *parent, QString dirPath) : QThread{parent}
{
    currBuffer = BUFFER_1;
    //mSensorBuffer1.index = 0;
    //mSensorBuffer2.index = 0;
    AverageIntervalSec = mICPSettings->getCurrentAverageIntervalSec();
    buffSize =(int) (1000.0 / TIME_INTERVAL_FOR_WRITE_ON_GRAPH * AverageIntervalSec);
    mAverageValue = 0;
    mAverageCount = 0;
    mAverageSum = 0;
    srand(5);    
    qDebug() << "buffSize" << buffSize;
    CurrDataForAverage = new double[buffSize];
    first = 0;
    last = 0;
    sum = 0;
    cnt = 0;
    SPIData.setFileName(dirPath + "/SPI_Data.txt");
    isStopSensorData = false;
}


SensorDataManager::~SensorDataManager()
{
}



#define indexPressureH2O 13.595
unsigned short data = 0;
void SensorDataManager::run()
{
  currIndex = -1; // увеличим в функции записи, т.е. с нуля начинаем запись

  isStopped = false;
  isRunning = true;
  isRecording = false;
  qDebug() << "SensorDataManager started";

  initTimerGraph();
  initTimerRecord();
  currIndexDrawGraph = 0;

  while (isRunning)
  {
      if (isRecording)
      {
          if (getCurrentTimeStamp_ms() >= timerForSetInGraph) // Add Data To Graph
          {
              startTimerGraph();
              READ_SPI();
              if (mICPSettings->getCurrentPressureIndex() == 1)
              {
                  data *= indexPressureH2O;
              }
              mAverageValue = calcAverage(data);
              emit(printDataOnGraph((unsigned int)(currIndexDrawGraph * TIME_INTERVAL_FOR_WRITE_ON_GRAPH), data));
              currIndexDrawGraph++;
              //emit(averageReady(mAverageValue));
              qDebug() << "manager x*1000" << currIndexDrawGraph * TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
          }

          if (getCurrentTimeStamp_ms() >= timerForRecordInFile) // Add Data In File
          {
              startTimerRecord();
              currIndex++; // Сначала увеличим индекс , чтобы индекс указывал на последний элемент
              READ_SPI();

              if (mICPSettings->getCurrentPressureIndex() == 1)
              {
                  data *= indexPressureH2O;
              }
              uint32_t tempBuf = currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE;
              SPIData.write((char*)&tempBuf, sizeof(tempBuf));
              SPIData.write((char*)&data, sizeof(data));
          }
      }
      else
      {
          if (isStopSensorData == false)
          {
              if (getCurrentTimeStamp_ms() >= timerForSetInGraph) // Add Data To Graph
              {
                  currIndex ++;
                  startTimerGraph();
                  READ_SPI();
                  if (mICPSettings->getCurrentPressureIndex() == 1)
                  {
                      data *= indexPressureH2O;
                  }
                  qDebug() << "ttt" << currIndex * TIME_INTERVAL_FOR_WRITE_ON_GRAPH;
                  mAverageValue = calcAverage(data);
                  emit(printDataOnGraph((unsigned int)(currIndex * TIME_INTERVAL_FOR_WRITE_ON_GRAPH), data));
                  //emit(averageReady(mAverageValue));
              }
          }
          else qDebug() << "isStopSensorData";
      }
  }  // End of while()
  SPIData.close();
  isStopped = true;

  qDebug() << "SensorDataManager stopped";
}


double SensorDataManager::calcAverage(int data)
{
    //qDebug() << "cnt" << cnt;
    //qDebug() << "data" << data;
    first = (first + 1) % buffSize;
    sum += data;
    if (cnt < buffSize)
    {
        cnt++;
    }
    else
    {
        last = (last + 1) % buffSize;
        sum -= CurrDataForAverage[last];
    }
    CurrDataForAverage[first] = data;
    //qDebug() << "average" << sum/cnt;
    return sum/cnt;
}

void SensorDataManager::startRecord()
{
    SPIData.open(QIODevice::WriteOnly | QIODevice::Append);
    initTimerGraph();
    initTimerRecord();
    currIndex = -1;
    currIndexDrawGraph = 0;
    isRecording = true;
}

//              if (currBuffer == BUFFER_1)
//              {
//                  mSensorBuffer1.record[mSensorBuffer1.index].data = data;
//                  mSensorBuffer1.record[mSensorBuffer1.index++].timeStamp = currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE;
//                  if (mSensorBuffer1.index == MAXBUFFERSIZE)
//                  {
//                      selectCurrentBufferForRecord(BUFFER_2);
//                      emit(writeBufferToFile());
//                  }
//              }
//              else
//              {
//                  mSensorBuffer2.record[mSensorBuffer2.index].data = data;
//                  mSensorBuffer2.record[mSensorBuffer2.index++].timeStamp = currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE;
//                  if (mSensorBuffer2.index == MAXBUFFERSIZE)
//                  {
//                      selectCurrentBufferForRecord(BUFFER_1);
//                      emit(writeBufferToFile());
//                  }
//              }
