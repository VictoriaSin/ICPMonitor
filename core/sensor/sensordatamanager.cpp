#include "sensordatamanager.h"
#include "impl/fileimitsensor.h"
#include "../app/global_define.h"

#include "stdlib.h"

#include <QVector>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>


#define numPosDataForPrint 16

uint8_t currBuffer = 1;
mSPIBuffer mSensorBuffer1;
mSPIBuffer mSensorBuffer2;


SensorDataManager::SensorDataManager()
{
    mSensorBuffer1.index = 0;
    mSensorBuffer2.index = 0;
    currIndex = 0;
    msCount = 0;

    getSPITimer = new QTimer;
    QVector<uint64_t> onlyPressure;
    srand(5);
#define maxGenContainer 24000
    for (int i = 0; i < maxGenContainer; i++)
    {
        onlyPressure.append(10 + rand() % 20);
    }

    getSPITimer->setInterval(2);
    getSPITimer->start();
    connect(getSPITimer, &QTimer::timeout, [this, onlyPressure](){addDataToBuffer(onlyPressure[currIndex]);});
}

SensorDataManager::~SensorDataManager()
{

}

void SensorDataManager::run()
{
    //QVector<QPair<float, float>> mSensorData;

    while (1)
    {

    }
}

bool SensorDataManager::readDataFromSensor()
{

}


bool SensorDataManager::addDataToBuffer(uint64_t data)
{
    msCount += 2;
    if (currBuffer == 1)
    {
        if (mSensorBuffer1.index == maxBufferSize)
        {
            currBuffer = 2;
        }
        emit(writeBufferToFile());
    }
    else
    {
        if (mSensorBuffer2.index == maxBufferSize)
        {
            currBuffer = 1;
        }
        emit(writeBufferToFile());
    }

    if (currBuffer == 1)
    {
        mSensorBuffer1.data[mSensorBuffer1.index] = data;
        mSensorBuffer1.index ++;
    }
    else
    {
        mSensorBuffer2.data[mSensorBuffer1.index] = data;
        mSensorBuffer2.index ++;
    }
    if (currIndex % numPosDataForPrint == 0)
    {
        emit(printDataOnGraph(msCount, data));
    }
    currIndex++;
    if (currIndex == maxGenContainer)
    {
        stopReading();
    }
    return true;
}

void SensorDataManager::stopReading()
{
    //корректное завершение работы с датчиком + завершение таймера
    emit(writeBufferToFile());
    getSPITimer->stop();
}
