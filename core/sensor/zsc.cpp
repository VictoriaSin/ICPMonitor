#include "zsc.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "string.h"
#include <QThread>
#include <qdebug.h>

ZSC::ZSC()
{
  mSetting.mode   = (uint32_t)SPI_MODE_2;
  mSetting.speed  = 500000;
  mSetting.bits   = (uint8_t)8;
  mSetting.delay  = (uint16_t)0;
  mSetting.lsb    = (uint8_t)0;

  memset((char*)&mMessageTX, 0, sizeof(mMessageTX));
  mMessageTX.tx_buf         = (unsigned long)&txBuffer;
  mMessageTX.speed_hz       = mSetting.speed;
  mMessageTX.bits_per_word  = mSetting.bits;

  memset((char*)&mMessageRX, 0, sizeof(mMessageRX));
  mMessageRX.rx_buf         = (unsigned long)&rxBuffer;
  mMessageRX.speed_hz       = mSetting.speed;
  mMessageRX.bits_per_word  = mSetting.bits;

  spi_name = "/dev/spidev0.0";

  initPins();
  spi_open();
  initZSC();
  qDebug("ZSC class init");
}
void ZSC::initZSC()
{
#ifndef PC_BUILD

  bool result;
  QThread::msleep(100);
  result = spi_open(); if (result == false) { qDebug() << "errror spi_open()"; return; }

  resetZSC();
  SPI_CMD(START_CM);

  u16 regs[32];
  char rBuf[200];

  // set registers value DANYA

  for (uint i=0; i<32; i++)
  {
      regsValue[i] = initRegs[i];//mICPSettings->getRegValues()[i];
      spi_saveReg(i, regsValue[i], ZSC_RAM);
  }
qDebug() << "1";
  // show registers
  for (u8 reg = 0; reg < 32; reg++)
  {
    regs[reg] = spi_getReg(reg, ZSC_RAM);
    sprintf(rBuf, "Reg[0x%02X]=0x%04X", reg, regs[reg]);
    qDebug() << rBuf;
  }
 // QThread::msleep(2000);
  SPI_CMD(START_CYC_RAM); QThread::msleep(10); // Start measurement cycle including initialization from RAM
  SPI_CMD(START_NOM);     QThread::msleep(100);
#endif
}
void ZSC::resetRegsValues()
{
#ifndef PC_BUILD
    resetZSC();
    SPI_CMD(START_CM);
    for (uint i=0; i<32; i++)
    {        
        regsValue[i] = mICPSettings->getRegValues()[i];
        spi_saveReg(i, regsValue[i], ZSC_RAM);
    }
    SPI_CMD(START_CYC_RAM); QThread::msleep(10); // Start measurement cycle including initialization from RAM
    SPI_CMD(START_NOM);     QThread::msleep(100);
#endif
}
void ZSC::terminate()
{
#ifndef PC_BUILD
  spi_close();
#endif
}
bool ZSC::spi_open()
{
  mIsOpen = false;
  mFd = ::open(spi_name.c_str(), O_RDWR);
  if (mFd < 0) { return false; }
  // Устанавливаем режим чтения
  if (ioctl(mFd, SPI_IOC_WR_MODE, &mSetting.mode) < 0) { return false; }
  // Устанавливаем режим записи
  if (ioctl(mFd, SPI_IOC_RD_MODE, &mSetting.mode) < 0) { return false; }
  // Устанавливаем количество бит на слово при записи
  if (ioctl(mFd, SPI_IOC_WR_BITS_PER_WORD, &mSetting.bits) < 0) { return false; }
  // Устанавливаем количество бит на слово при чтении
  if (ioctl(mFd, SPI_IOC_RD_BITS_PER_WORD, &mSetting.bits) < 0) { return false; }
  // Устанавливаем битовое выравнивание для чтения
  if (ioctl(mFd, SPI_IOC_RD_LSB_FIRST, &mSetting.bits) < 0) { return false; }
  // Устанавливаем битовое выравнивание для записи
  if (ioctl(mFd, SPI_IOC_WR_LSB_FIRST, &mSetting.bits) < 0) { return false; }
  // Устанавливаем максимальную скорость передачи данных по SPI в Гц
  if (ioctl(mFd, SPI_IOC_WR_MAX_SPEED_HZ, &mSetting.speed) < 0) { return false; }
  // Устанавливаем максимальную скорость чтения данных по SPI в Гц
  if (ioctl(mFd, SPI_IOC_RD_MAX_SPEED_HZ, &mSetting.speed) < 0) { return false; }
  mIsOpen = true;
  return true;
}
bool ZSC::spi_close()
{
  if (!mIsOpen) { return false; }
  mIsOpen = false;
  if(::close(mFd) != 0) { mFd = -1; return false; }
  mFd = -1;
  return true;
}
bool ZSC::spi_readSPI()
{
  CS_L();
  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageRX) < 1) { CS_H(); return false; }
  CS_H();
  return true;
}
bool ZSC::spi_write()
{
  CS_L();
  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageTX) < 1) { CS_H(); return false; }
  CS_H();
  return true;
}
bool ZSC::SPI_CMD(u8 CMD)
{
  txBuffer[0] = SLAVE_ADDR_W;
  txBuffer[1] = CMD;
  mMessageTX.len = 2;
  return spi_write();
}
u16 ZSC::spi_getReg(u8 reg, u8 memType)
{
  SPI_CMD(reg + memType);
  txBuffer[0] = SLAVE_ADDR_R;
  CS_L();
  mMessageTX.len = 1;
  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageTX) < 1) { CS_H(); return false; }
  mMessageRX.len = 2;
  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageRX) < 1) { CS_H(); return false; }
  CS_H();
  return ((u16)(rxBuffer[0]<<8) + rxBuffer[1]);
}
bool ZSC::spi_saveReg(u8 reg, u16 data, u8 memType)
{
  txBuffer[0] = SLAVE_ADDR_W;
  txBuffer[1] = reg + memType + 0x70;
  txBuffer[2] = (u8)(data >> 8);
  txBuffer[3] = (u8)data;
  mMessageTX.len = 4;
  CS_L();
  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageTX) < 1) { CS_H(); return false; }
  CS_H();
}

#ifndef PC_BUILD
void ZSC::spi_oneShot()
{
  CS_L();
  txBuffer[0] = SLAVE_ADDR_R;
  mMessageTX.len = 1;
  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageTX) < 1) { CS_H(); return; }
  mMessageRX.len = 2;
  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageRX) < 1) { CS_H(); return; }
  data[0] = (u16)(rxBuffer[0]<<8) + rxBuffer[1];
  CS_H();

}
#endif
u16 ZSC::signature(u16* eepcont, u16 N)
{
  u16 sign, poly, p, x, i, j;
  sign = 0; poly = POLYNOM;
  for (i = 0; i < N; i++)
  {
    sign ^= eepcont[i];
    p=0;
    x = sign & poly;
    for (j = 0; j < 16; j++, p ^= x, x >>= 1);
    sign <<= 1; sign += (p&1);
  }
  return(~sign);
}
void ZSC::resetZSC()
{
#ifndef PC_BUILD
  FILE *fpReset_2 = nullptr;
  fpReset_2=fopen(GPIO_PIN_PATH_RESET"/value","w");
  fprintf(fpReset_2,"1");
  fclose(fpReset_2);

  QThread::msleep(100);

  fpReset_2=fopen(GPIO_PIN_PATH_RESET"/value","w");
  fprintf(fpReset_2,"0");
  fclose(fpReset_2);
#endif
}
void ZSC::initPins()
{
#ifndef PC_BUILD
  FILE *fpReset = nullptr;
  fpReset=fopen(GPIO_PIN_PATH_RESET"/direction","w");
  fprintf(fpReset,"out");
  fclose(fpReset);

  fpReset=fopen(GPIO_PIN_PATH_RESET"/active_low","w");
  fprintf(fpReset,"0");
  fclose(fpReset);

  fpReset=fopen(GPIO_PIN_PATH_RESET"/value","w");
  fprintf(fpReset,"0");
  fclose(fpReset);

  fpCS=fopen(GPIO_PIN_PATH_CS"/direction","w");
  fprintf(fpCS,"out");
  fclose(fpCS);

  fpCS=fopen(GPIO_PIN_PATH_CS"/active_low","w");
  fprintf(fpCS,"0");
  fclose(fpCS);

  CS_H();

  qDebug() << "Pins init";
#endif
}
