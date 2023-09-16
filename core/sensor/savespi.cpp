#include "savespi.h"
#include "../app/global_define.h"

#include <QDebug>

extern uint8_t currBuffer;


SaveSPI::SaveSPI(QObject *parent, QString dirPath) : QThread{parent}
{
    SPIData.setFileName(dirPath + "/SPI_Data.txt");
}

SaveSPI::~SaveSPI()
{

}
void SaveSPI::run()
{
    isStopped = false;
    isRunning = true;
    qDebug() << "SaveSPI started";
    while (isRunning)
    {

    }
    isStopped = true;
    fillFile();
    qDebug() << "SaveSPI stopped";
}

void SaveSPI::fillFile()
{
    SPIData.open(QIODevice::WriteOnly | QIODevice::Append);
    if (currBuffer == BUFFER_2)
    {
        SPIData.write((char*)&mSensorBuffer1.record, mSensorBuffer1.index * sizeof(_mSPIData));
        mSensorBuffer1.index = 0;
    }
    else
    {
        SPIData.write((char*)&mSensorBuffer2.record, mSensorBuffer2.index * sizeof(_mSPIData));
        mSensorBuffer2.index = 0;
    }
    SPIData.close();
}
