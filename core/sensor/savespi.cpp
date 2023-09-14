#include "savespi.h"
#include "../app/global_define.h"

extern uint8_t currBuffer;


SaveSPI::SaveSPI()
{
    SPIData.setFileName("/media/SPI_Data.txt");
}

void SaveSPI::run()
{

}

void SaveSPI::fillFile()
{
    SPIData.open(QIODevice::WriteOnly | QIODevice::Append);
    if (currBuffer == 1)
    {
        SPIData.write((char*)mSensorBuffer1.data, mSensorBuffer1.index * sizeof(mSPIBuffer));
        mSensorBuffer1.index = 0;
    }
    else
    {
        SPIData.write((char*)mSensorBuffer2.data, mSensorBuffer2.index * sizeof(mSPIBuffer));
        mSensorBuffer2.index = 0;
    }
    SPIData.close();
}
