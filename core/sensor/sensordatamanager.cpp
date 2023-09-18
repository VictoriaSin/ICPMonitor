#include "sensordatamanager.h"
#include "impl/fileimitsensor.h"
#include "../app/global_define.h"

#include "stdlib.h"

#include <QVector>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>


#define selectCurrentBufferForRecord(_BUF) { currBuffer = _BUF;}
uint8_t currBuffer = BUFFER_1;
//mSPIBuffer mSensorBuffer1;
//mSPIBuffer mSensorBuffer2;
_mSPIBuffer mSensorBuffer1, mSensorBuffer2;


SensorDataManager::SensorDataManager(QObject *parent) : QThread{parent}
{
    currBuffer = BUFFER_1;
    mSensorBuffer1.index = 0;
    mSensorBuffer2.index = 0;
    mAverageValue = 0;
    mAverageCount = 0;
    mAverageSum = 0;
    srand(5);
}

SensorDataManager::~SensorDataManager()
{
}

#define TIME_INTERVAL_FOR_RECORD_IN_FILE (3)
#define TIME_INTERVAL_FOR_WRITE_ON_GRAPH (40) //40 миллисекунд - 25 раз в секунду

#define initTimerGraph() {timerForSetInGraph = getCurrentTimeStamp_ms() + TIME_INTERVAL_FOR_WRITE_ON_GRAPH;}
#define initTimerRecord() {timerForRecordInFile =  getCurrentTimeStamp_ms() + TIME_INTERVAL_FOR_RECORD_IN_FILE;}
#define startTimerGraph() {timerForSetInGraph += TIME_INTERVAL_FOR_WRITE_ON_GRAPH;}
#define startTimerRecord() {timerForRecordInFile += TIME_INTERVAL_FOR_RECORD_IN_FILE;}

#define READ_SPI() data = (10 + rand() % 20) // Потом будем брать значение из потока SPI

unsigned short data = 0;
void SensorDataManager::run()
{
  volatile int32_t currIndex = -1; // увеличим в функции записи, т.е. с нуля начинаем запись

  isStopped = false;
  isRunning = true;
  isRecording = false;
  isStartRecord = false;
  qDebug() << "SensorDataManager started";

  initTimerGraph();
  initTimerRecord();
  uint32_t currIndexDrawGraph = 0;

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
                  data *= 13.595;
              }
              emit(printDataOnGraph((unsigned int)(currIndexDrawGraph * TIME_INTERVAL_FOR_WRITE_ON_GRAPH), data));
              currIndexDrawGraph++;
              emit(averageReady(mAverageValue));
          }

          if (getCurrentTimeStamp_ms() >= timerForRecordInFile) // Add Data In File
          {
              startTimerRecord();
              currIndex++; // Сначала увеличим индекс , чтобы индекс указывал на последний элемент
              READ_SPI();
              if (mICPSettings->getCurrentPressureIndex() == 1)
              {
                  data *= 13.595;
              }
              if (currBuffer == BUFFER_1)
              {
                  mSensorBuffer1.record[mSensorBuffer1.index].data = data;
                  mSensorBuffer1.record[mSensorBuffer1.index++].timeStamp = currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE;
                  if (mSensorBuffer1.index == MAXBUFFERSIZE)
                  {
                      selectCurrentBufferForRecord(BUFFER_2);
                      emit(writeBufferToFile());
                  }
              }
              else
              {
                  mSensorBuffer2.record[mSensorBuffer2.index].data = data;
                  mSensorBuffer2.record[mSensorBuffer2.index++].timeStamp = currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE;
                  if (mSensorBuffer2.index == MAXBUFFERSIZE)
                  {
                      selectCurrentBufferForRecord(BUFFER_1);
                      emit(writeBufferToFile());
                  }
              }
              READ_SPI();
              if (mICPSettings->getCurrentPressureIndex() == 1)
              {
                  data *= 13.595;
              }
              mAverageSum += (double)(data);
              mAverageCount++;
              mAverageValue = mAverageSum / mAverageCount;
          }
      }
      else
      {
          if (isStartRecord)
          {
              initTimerGraph();
              initTimerRecord();
              isRecording = true;
              isStartRecord = false;
              currIndex = -1;
          }
          else
          {
              if (getCurrentTimeStamp_ms() >= timerForSetInGraph) // Add Data To Graph
              {
                  currIndex ++;
                  startTimerGraph();
                  READ_SPI();
                  if (mICPSettings->getCurrentPressureIndex() == 1)
                  {
                      data *= 13.595;
                  }
                  emit(printDataOnGraph((unsigned int)(currIndex * TIME_INTERVAL_FOR_WRITE_ON_GRAPH), data));
                  emit(averageReady(mAverageValue));

              }
              if (getCurrentTimeStamp_ms() >= timerForRecordInFile) // Add Data In File
              {
                  READ_SPI();
                  if (mICPSettings->getCurrentPressureIndex() == 1)
                  {
                      data *= 13.595;
                  }
                  mAverageSum += (double)(data);
                  mAverageCount++;
                  mAverageValue = mAverageSum / mAverageCount;
              }
          }
      }
  }  // End of while()
  isStopped = true;
  qDebug() << "SensorDataManager stopped";
}
