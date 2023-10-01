
//#include "owi_protocol.h"
//#include <QThread>
//#include "stdio.h"
//#include "string.h"
//int mFd {-1};
//bool mIsOpen = false;
//char spi_name[] ="/dev/spidev0.0";
//struct SPISetting
//{
//    uint32_t mode;
//    uint32_t speed;
//    uint8_t  bits;
//    uint16_t delay;
//    uint8_t  lsb;
//};
//struct SPISetting mSetting;
//u8 txBuffer[32];
//u8 rxBuffer[32];
//struct spi_ioc_transfer mMessageTX;
//struct spi_ioc_transfer mMessageRX;

//bool spi_open()
//{
//  mIsOpen = false;
//  mFd = ::open(spi_name, O_RDWR);
//  if (mFd < 0) { return false; }
//  // Устанавливаем режим чтения
//  if (ioctl(mFd, SPI_IOC_WR_MODE, &mSetting.mode) < 0) { return false; }
//  // Устанавливаем режим записи
//  if (ioctl(mFd, SPI_IOC_RD_MODE, &mSetting.mode) < 0) { return false; }
//  // Устанавливаем количество бит на слово при записи
//  if (ioctl(mFd, SPI_IOC_WR_BITS_PER_WORD, &mSetting.bits) < 0) { return false; }
//  // Устанавливаем количество бит на слово при чтении
//  if (ioctl(mFd, SPI_IOC_RD_BITS_PER_WORD, &mSetting.bits) < 0) { return false; }
//  // Устанавливаем битовое выравнивание для чтения
//  if (ioctl(mFd, SPI_IOC_RD_LSB_FIRST, &mSetting.bits) < 0) { return false; }
//  // Устанавливаем битовое выравнивание для записи
//  if (ioctl(mFd, SPI_IOC_WR_LSB_FIRST, &mSetting.bits) < 0) { return false; }
//  // Устанавливаем максимальную скорость передачи данных по SPI в Гц
//  if (ioctl(mFd, SPI_IOC_WR_MAX_SPEED_HZ, &mSetting.speed) < 0) { return false; }
//  // Устанавливаем максимальную скорость чтения данных по SPI в Гц
//  if (ioctl(mFd, SPI_IOC_RD_MAX_SPEED_HZ, &mSetting.speed) < 0) { return false; }
//  mIsOpen = true;
//  return true;
//}
//bool spi_close()
//{
//  if(::close(mFd) != 0) { mFd = -1; return false; }
//  mIsOpen = false;
//  return true;
//}

//#define SLAVE_ADDR_W 0xF0
//#define SLAVE_ADDR_R 0xF1
//FILE *fpCS = nullptr;

//#define CS_L()
//{ fpCS=fopen(GPIO_PIN_PATH_OWI"/value","w");
//  fprintf(fpCS,"0");
//  fclose(fpCS); }

//#define CS_H()
//{ fpCS=fopen(GPIO_PIN_PATH_OWI"/value","w");
//  fprintf(fpCS,"1");
//  fclose(fpCS); }

//bool spi_readSPI()
//{
//  CS_L();
//  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageRX) < 1) { CS_H(); return false; }
//  CS_H();
//  return true;
//}

//bool spi_write()
//{
//  CS_L();
//  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageTX) < 1) { CS_H(); return false; }
//  CS_H();
//  return true;
//}

//bool SPI_CMD(u8 CMD)
//{

//  txBuffer[0] = SLAVE_ADDR_W;
//  txBuffer[1] = CMD;
//  mMessageTX.len = 2;
//  return spi_write();
//}

//u16 SPI_getReg(u8 reg, u8 memType)
//{
//  SPI_CMD(reg + memType);
//  txBuffer[0] = SLAVE_ADDR_R;
//  CS_L();
//  mMessageTX.len = 1;
//  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageTX) < 1) { CS_H(); return false; }
//  mMessageRX.len = 2;
//  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageRX) < 1) { CS_H(); return false; }
//  CS_H();
//  return ((u16)(rxBuffer[0]<<8) + rxBuffer[1]);
//}

//bool SPI_getArray(u16 *data)
//{
//  CS_L();
//  txBuffer[0] = SLAVE_ADDR_R;
//  mMessageTX.len = 1;
//  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageTX) < 1) { CS_H(); return false; }
//  mMessageRX.len = 4;
//  if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessageRX) < 1) { CS_H(); return false; }
//  data[0] = ((u16)(rxBuffer[0]<<8) + rxBuffer[1]);
//  data[1] = ((u16)(rxBuffer[2]<<8) + rxBuffer[3]);
//  CS_H();
//  return true;
//}


//void Test()
//{
//  bool result;
//  mSetting =
//  {
//    .mode   = (uint32_t)SPI_MODE_2,
//    .speed  = 500000,
//    .bits   = (uint8_t)8,
//    .delay  = (uint16_t)0,
//    .lsb    = (uint8_t)0,
//  };
//  memset((char*)&mMessageTX, 0, sizeof(mMessageTX));
//  mMessageTX.tx_buf         = (unsigned long)&txBuffer;
//  mMessageTX.speed_hz       = mSetting.speed;
//  mMessageTX.bits_per_word  = mSetting.bits;

//  memset((char*)&mMessageRX, 0, sizeof(mMessageRX));
//  mMessageRX.rx_buf         = (unsigned long)&rxBuffer;
//  mMessageRX.speed_hz       = mSetting.speed;
//  mMessageRX.bits_per_word  = mSetting.bits;


//  QThread::msleep(100);
//  result = spi_open(); if (result == false) { qDebug() << "errror spi_open()"; return; }

//  ResetZSC();
//  SPI_CMD(START_CM);

//  u16 regs[32];

//    for (u8 reg = 0; reg < 32; reg++)
//    {
//      regs[reg] = SPI_getReg(reg, ZSC_RAM);
//    }
//    qDebug() << regs[0] << regs[1]<< regs[2]<< regs[3];
//    QThread::msleep(100);

//  SPI_CMD(START_CYC_RAM); QThread::msleep(10); // Start measurement cycle including initialization from RAM
//  SPI_CMD(START_NOM);     QThread::msleep(100);
//  u16 data[2];
//  char rBuf[200];
//  while(1)
//  {
//    SPI_getArray(data);
//    sprintf(rBuf, "data1 = %d\tdata2 = %d", data[0], data[1]);
//    qDebug() << rBuf;
//    QThread::sleep(1);
//  }

////  result = spi_close();
////  if (result == false)
////  {
////    qDebug() << "errror spi_close()";
////    return;
////  }
////  qDebug() << "cool";
//}
////echo 25 > /sys/class/gpio/export // RESET PIN
////echo out > /sys/class/gpio/gpio25/direction // OUT
////echo 1 > /sys/class/gpio/gpio25/value      // SET
////echo 0 > /sys/class/gpio/gpio25/value      // RESET

////echo 26  > /sys/class/gpio/export           // OWI PIN
////echo out > /sys/class/gpio/gpio26/direction // OUT
////echo 1   > /sys/class/gpio/gpio26/value     // SET
////echo 0   > /sys/class/gpio/gpio26/value     // RESET

//#define GPIO_PIN_PATH_RESET "/sys/class/gpio/gpio25" // RESET
//#define GPIO_PIN_PATH_CS "/sys/class/gpio/gpio26"   // OWI

//#define PIN_RESET GPIO_PIN_PATH_RESET
//#define PIN_CS   GPIO_PIN_PATH_CS

//FILE *fp = nullptr;

//#define POLYNOM 0xA005
//u16 signature(u16* eepcont, u16 N)
//{
//  u16 sign, poly, p, x, i, j;
//  sign = 0; poly = POLYNOM;
//  for (i = 0; i < N; i++)
//  {
//    sign ^= eepcont[i];
//    p=0;
//    x = sign & poly;
//    for (j = 0; j < 16; j++, p ^= x, x >>= 1);
//    sign <<= 1; sign += (p&1);
//  }
//  return(~sign);
//}

//
//  LL_GPIO_SetOutputPin(GPIO_Reset_GPIO_Port, GPIO_Reset_Pin);
//  LL_mDelay(50);
//  LL_GPIO_ResetOutputPin(GPIO_Reset_GPIO_Port, GPIO_Reset_Pin);
//  LL_mDelay(5); // После сброса нужно немного подождать но не более 19 милсек (окно OWI 20 милсек)
//
//void ResetZSC()
//{
//  FILE *fpReset_2 = nullptr;
//  fpReset_2=fopen(GPIO_PIN_PATH_RESET"/value","w");
//  fprintf(fpReset_2,"1");
//  fclose(fpReset_2);

//  QThread::msleep(100);

//  fpReset_2=fopen(GPIO_PIN_PATH_RESET"/value","w");
//  fprintf(fpReset_2,"0");
//  fclose(fpReset_2);
//}

//void OWI_InitPins()
//{
//  FILE *fpReset = nullptr;
//  fpReset=fopen(GPIO_PIN_PATH_RESET"/direction","w");
//  fprintf(fpReset,"out");
//  fclose(fpReset);

//  FILE *fpReset_1 = nullptr;
//  fpReset_1=fopen(GPIO_PIN_PATH_RESET"/active_low","w");
//  fprintf(fpReset_1,"0");
//  fclose(fpReset_1);

//  FILE *fpReset_2 = nullptr;
//  fpReset_2=fopen(GPIO_PIN_PATH_RESET"/value","w");
//  fprintf(fpReset_2,"0");
//  fclose(fpReset_2);

//  fpCS=fopen(GPIO_PIN_PATH_OWI"/direction","w");
//  fprintf(fpCS,"out");
//  fclose(fpCS);


//  fpCS=fopen(GPIO_PIN_PATH_OWI"/active_low","w");
//  fprintf(fpCS,"0");
//  fclose(fpCS);

//  CS_H();

//  qDebug() << "Pins init";
//}

////#define DEL_10 82 // Выбирается методом подбора экспериментально либо используются спец возможности того или иного процессора
//////void OWI_delay_us(u16 usDelay)
//////{
//////  volatile u32 i;
//////  switch (usDelay)
//////  {
//////    case 10:  { i =  DEL_10; break; }
//////    case 20:  { i = (DEL_10 << 1) + 5;   break; }
//////    case 30:  { i = (DEL_10 * 3)  + 8;   break; }
//////    case 170: { i = (DEL_10 * 17) + 50;  break; }
//////    default:  { i = 1000; break;}
//////  }
//////  while(i!=0) { i--; }
//////}

////__inline static void OWI_writeByte(u8 sByte)
////{
////  for (u8 i = 0; i < 8; i++)
////  {
////    if ((sByte & 0x80) != 0)
////    {
////      OWI_H; //QThread::usleep(30);
////      OWI_L; //QThread::usleep(10);
////    }
////    else
////    {
////      OWI_H; //QThread::usleep(10);
////      OWI_L; //QThread::usleep(30);
////    }
////    sByte <<= 1;
////  }
////}
////__inline static void OWI_writeWord(u16 sWord)
////{
////  for (u8 i = 0; i < 16; i++)
////  {
////    if ((sWord & 0x8000) != 0) // Отправка "1"
////    {
////      OWI_H; //QThread::usleep(30);
////      OWI_L; //QThread::usleep(10);
////    }
////    else                      // Отправка "0"
////    {
////      OWI_H; //QThread::usleep(10);
////      OWI_L; //QThread::usleep(30);
////    }
////    sWord <<= 1;
////  }
////}
//////__inline static u16 OWI_readWord(void)
//////{
//////  register u16 temp = 0;      // Принимаемое значение (чтобы не работать по указателям)
//////  for (register u8 i = 0; i < 16; i++)
//////  {
//////    temp <<= 1;
//////    while (LINE_IS_0);        // Ждем пока диния не станет в "1"
//////    OWI_delay_us(20);
//////    if (LINE_IS_1) { temp++; }
//////    while (LINE_IS_1);        // Ждем пока диния не станет в "0"
//////  }
//////  return temp;
//////}


////void OWI_CMD_Data16(u8 CMD, u16 data)
////{
////  OWI_L;  //QThread::usleep(20);       // Start OWI
////  OWI_writeByte(SLAVE_ADDR_W);    // Send DeviceSlave Address WRITE
////  OWI_writeByte(CMD);             // CMD send
////  OWI_writeWord(data);            // write data 16 bits
////  OWI_H;  //QThread::usleep(170);      // Hold time stop condition in HIGH level
////}
////void OWI_CMD(u8 CMD)
////{
////  OWI_L;  //QThread::usleep(20);       // Start OWI
////  OWI_writeByte(SLAVE_ADDR_W);    // Send DeviceSlave Address WRITE
////  OWI_writeByte(CMD);             // CMD send
////  OWI_H;  //QThread::usleep(170);      // Hold time stop condition in HIGH level
////}

////void Test()
////{
////  fp=fopen(GPIO_PIN_PATH_OWI"/value","rw");
////  u8 buf[10];
////  fread(buf, 1,1, fp);
////  //u8 bytes_read = read(fp, buf, 1);
////  //printf(outtt);
////  qDebug() << buf[0];
////  fclose(fp);
////}

////void OWI_readData(u16* data)
////{
////  OWI_L;  OWI_delay_us(20);       // Start OWI
////  OWI_writeByte(SLAVE_ADDR_R);    // Send DeviceSlave Address READ
////  SET_LINE_INPUT;                 // Меняем выход на вход (подтяжка вверх LL_GPIO_PULL_UP)
////  *data = OWI_readWord();         // read 16 bits
////  SET_LINE_OUTPUT;                // Меняем вход на выход
////  OWI_delay_us(170);  OWI_H;      // Hold time stop condition is LOW level
////}
////
////u16 OWI_getReg(u8 reg, u8 memType)
////{
////  u16 regValue;
////  OWI_CMD(reg + memType);
////  OWI_readData(&regValue);
////  return regValue;
////}
