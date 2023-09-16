#include "sensordatamanager.h"
#include "impl/fileimitsensor.h"
#include "../app/global_define.h"

#include "stdlib.h"

#include <QVector>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>



#define numPosDataForPrint 16

#define selectCurrentBufferForRecord(_BUF) { currBuffer = _BUF;}
uint8_t currBuffer = BUFFER_1;
mSPIBuffer mSensorBuffer1;
mSPIBuffer mSensorBuffer2;



SensorDataManager::SensorDataManager(QObject *parent) : QThread{parent}
{
    currBuffer = BUFFER_1;
    mSensorBuffer1.index = 0;
    mSensorBuffer2.index = 0;
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


unsigned int data = 0;
void SensorDataManager::run()
{
  int32_t currIndex = -1; // увеличим в функции записи, т.е. с нуля начинаем запись
  isStopped = false;
  isRunning = true;

  qDebug() << "SensorDataManager started";

  initTimerGraph();
  initTimerRecord();

  while (isRunning)
  {
    if (getCurrentTimeStamp_ms() >= timerForSetInGraph) // Add Data To Graph
    {
      startTimerGraph();
      emit(printDataOnGraph((unsigned int)(currIndex * TIME_INTERVAL_FOR_RECORD_IN_FILE), data));
    }

    if (getCurrentTimeStamp_ms() >= timerForRecordInFile) // Add Data In File
    {
      startTimerRecord();
      currIndex++; // Сначала увеличим индекс , чтобы индекс указывал на последний элемент
      data = (10 + rand() % 20); // Потом будем брать значение из потока SPI
      if (currBuffer == BUFFER_1)
      {
        mSensorBuffer1.data[mSensorBuffer1.index++] = data;
        if (mSensorBuffer1.index == MAXBUFFERSIZE)
        {
          selectCurrentBufferForRecord(BUFFER_2);
          emit(writeBufferToFile());
        }
      }
      else
      {
        mSensorBuffer2.data[mSensorBuffer2.index++] = data;
        if (mSensorBuffer2.index == MAXBUFFERSIZE)
        {
          selectCurrentBufferForRecord(BUFFER_1);
          emit(writeBufferToFile());
        }
      }
    }
  } // End of while()

  //emit(writeBufferToFile()); // остаток записи перенес в класс savespi иначе хрень получается
  QThread::msleep(100);
  isStopped = true;
  qDebug() << "SensorDataManager stopped";
}
