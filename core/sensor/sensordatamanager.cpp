#include "sensordatamanager.h"
#include "impl/fileimitsensor.h"
#include "../app/global_define.h"

#include "stdlib.h"

#include <QVector>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>



#define numPosDataForPrint 16
#define BUFFER_1 1
#define BUFFER_2 2
#define selectCurrentBufferForRecord(_BUF) { currBuffer = _BUF;}
uint8_t currBuffer = BUFFER_1;
mSPIBuffer mSensorBuffer1;
mSPIBuffer mSensorBuffer2;


float intervalCountPerSec = 1000.0/MAXBUFFERSIZE;
SensorDataManager::SensorDataManager(QObject *parent) : QThread{parent}
{
//SensorDataManager::SensorDataManager()
//{
    getSPITimer = new QTimer;
    getSPITimer->setInterval(2);
    connect(getSPITimer, &QTimer::timeout, this, &SensorDataManager::addDataToBuffer);
    getSPITimer->start();
    mSensorBuffer1.index = 0;
    mSensorBuffer2.index = 0;
    currIndex = 0;

}

SensorDataManager::~SensorDataManager()
{
}

void SensorDataManager::run()
{
    isStopped = false;
    isRunning = true;
    qDebug() << "SensorDataManager started";
    srand(5);
    while (isRunning)
    {

    }
    emit(writeBufferToFile());
    isStopped = true;
    qDebug() << "SensorDataManager stopped";
}


void SensorDataManager::addDataToBuffer()
{
    unsigned int data = (10 + rand() % 20);
    if (isRecording)
    {
        if (currBuffer == BUFFER_1)
        {
            mSensorBuffer1.data[mSensorBuffer1.index++] = data;
            if (mSensorBuffer1.index == MAXBUFFERSIZE)
            {
                selectCurrentBufferForRecord(BUFFER_2);
                qDebug() << data;
                emit(writeBufferToFile());
                mSensorBuffer1.index = 0;
            }
        }
        else
        {
            mSensorBuffer2.data[mSensorBuffer2.index++] = data;
            if (mSensorBuffer2.index == MAXBUFFERSIZE)
            {
                selectCurrentBufferForRecord(BUFFER_1);
                qDebug() << data;
                emit(writeBufferToFile());
                mSensorBuffer2.index = 0;
            }
        }
    }
    if ((currIndex % numPosDataForPrint) == 0)
    {
        unsigned int timeStamp = (unsigned int)(currIndex * intervalCountPerSec);
        emit(printDataOnGraph(timeStamp, data));
    }
    currIndex++;
}
//
//void SensorDataManager::stopReading()
//{
//    //корректное завершение работы с датчиком + завершение таймера
//
//}
//
