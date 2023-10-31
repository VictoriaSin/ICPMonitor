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

#ifdef PC_BUILD
u16 dataF[340] = {
12979, 13053, 13127, 13201, 13275, 13349, 13423, 13497, 13571, 13645, 13722, 13918, 14114, 14310, 14506, 14702, 14898, 15094, 15290, 15486,
15682, 15903, 16124, 16345, 16566, 16787, 17008, 17229, 17450, 17671, 17898, 18142, 18386, 18630, 18874, 19118, 19362, 19606, 19850, 20094,
20345, 20509, 20673, 20837, 21001, 21165, 21329, 21493, 21657, 21821, 21989, 22082, 22175, 22268, 22361, 22454, 22547, 22640, 22733, 22826,
22926, 22903, 22880, 22857, 22834, 22811, 22788, 22765, 22742, 22719, 22695, 22723, 22751, 22779, 22807, 22835, 22863, 22891, 22919, 22947,
22975, 22964, 22953, 22942, 22931, 22920, 22909, 22898, 22887, 22876, 22865, 22874, 22883, 22892, 22901, 22910, 22919, 22928, 22937, 22946,
22963, 22973, 22983, 22993, 23003, 23013, 23023, 23033, 23043, 23053, 23072, 23068, 23064, 23060, 23056, 23052, 23048, 23044, 23040, 23036,
23024, 22977, 22930, 22883, 22836, 22789, 22742, 22695, 22648, 22601, 22549, 22540, 22531, 22522, 22513, 22504, 22495, 22486, 22477, 22468,
22452, 22465, 22478, 22491, 22504, 22517, 22530, 22543, 22556, 22569, 22585, 22523, 22461, 22399, 22337, 22275, 22213, 22151, 22089, 22027,
21965, 21945, 21925, 21905, 21885, 21865, 21845, 21825, 21805, 21785, 21758, 21741, 21724, 21707, 21690, 21673, 21656, 21639, 21622, 21605,
21587, 21523, 21459, 21395, 21331, 21267, 21203, 21139, 21075, 21011, 20942, 20883, 20824, 20765, 20706, 20647, 20588, 20529, 20470, 20411,
20345, 20279, 20213, 20147, 20081, 20015, 19949, 19883, 19817, 19751, 19676, 19598, 19520, 19442, 19364, 19286, 19208, 19130, 19052, 18974,
18896, 18877, 18858, 18839, 18820, 18801, 18782, 18763, 18744, 18725, 18701, 18593, 18485, 18377, 18269, 18161, 18053, 17945, 17837, 17729,
17618, 17565, 17512, 17459, 17406, 17353, 17300, 17247, 17194, 17141, 17082, 16995, 16908, 16821, 16734, 16647, 16560, 16473, 16386, 16299,
16206, 16152, 16098, 16044, 15990, 15936, 15882, 15828, 15774, 15720, 15658, 15616, 15574, 15532, 15490, 15448, 15406, 15364, 15322, 15280,
15231, 15179, 15127, 15075, 15023, 14971, 14919, 14867, 14815, 14763, 14708, 14634, 14560, 14486, 14412, 14338, 14264, 14190, 14116, 14042,
13965, 13956, 13947, 13938, 13929, 13920, 13911, 13902, 13893, 13884, 13868, 13805, 13742, 13679, 13616, 13553, 13490, 13427, 13364, 13301,
13235, 13199, 13163, 13127, 13091, 13055, 13019, 12983, 12947, 12911, 12869, 12893, 12917, 12941, 12965, 12989, 13013, 13037, 13061, 14391
};
void ZSC::spi_oneShot()
{
  static i16 i = 0;
  static bool dir = true;
  static u16 posData = 0;
  static u16 amp = 0;

  if (posData == 0)
  {
    amp+= 55;
    if (amp >= 1000) amp = 0;
    if (dir) { i+= 1500; if (i == 6000) dir = false; }
    else { i-=1500; if (i == -6000) dir = true; }

  }
  data[0] = dataF[posData++] + amp + i;
  posData %= 340;
}
#else
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



