#include "owi_protocol.h"

//echo 25 > /sys/class/gpio/export // RESET PIN
//echo out > /sys/class/gpio/gpio25/direction // OUT
//echo 1 > /sys/class/gpio/gpio25/value      // SET
//echo 0 > /sys/class/gpio/gpio25/value      // RESET

//echo 26  > /sys/class/gpio/export           // OWI PIN
//echo out > /sys/class/gpio/gpio26/direction // OUT
//echo 1   > /sys/class/gpio/gpio26/value     // SET
//echo 0   > /sys/class/gpio/gpio26/value     // RESET


#define GPIO_PIN_PATH_RESET "/sys/class/gpio/gpio25" // RESET
#define GPIO_PIN_PATH_OWI "/sys/class/gpio/gpio26"   // OWI

FILE *fp = nullptr;

#define POLYNOM 0xA005
u16 signature(u16* eepcont, u16 N)
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

void OWI_InitPins()
{
  fp=fopen(GPIO_PIN_PATH_RESET"/direction","w");
  fprintf(fp,"out");
  fclose(fp);

  fp=fopen(GPIO_PIN_PATH_RESET"/active_low","w");
  fprintf(fp,"0");
  fclose(fp);
}
#define DEL_10 82 // Выбирается методом подбора экспериментально либо используются спец возможности того или иного процессора
void OWI_delay_us(u16 usDelay)
{
  volatile u32 i;
  switch (usDelay)
  {
    case 10:  { i =  DEL_10; break; }
    case 20:  { i = (DEL_10 << 1) + 5;   break; }
    case 30:  { i = (DEL_10 * 3)  + 8;   break; }
    case 170: { i = (DEL_10 * 17) + 50;  break; }
    default:  { i = 1000; break;}
  }
  while(i!=0) { i--; }
}

__inline static void OWI_writeByte(u8 sByte)
{
  for (u8 i = 0; i < 8; i++)
  {
    if ((sByte & 0x80) != 0)
    {
      OWI_H; OWI_delay_us(30);
      OWI_L; OWI_delay_us(10);
    }
    else
    {
      OWI_H; OWI_delay_us(10);
      OWI_L; OWI_delay_us(30);
    }
    sByte <<= 1;
  }
}
__inline static void OWI_writeWord(u16 sWord)
{
for (u8 i = 0; i < 16; i++)
  {
    if ((sWord & 0x8000) != 0) // Отправка "1"
    {
      OWI_H; OWI_delay_us(30);
      OWI_L; OWI_delay_us(10);
    }
    else                      // Отправка "0"
    {
      OWI_H; OWI_delay_us(10);
      OWI_L; OWI_delay_us(30);
    }
    sWord <<= 1;
  }
}
//__inline static u16 OWI_readWord(void)
//{
//  register u16 temp = 0;      // Принимаемое значение (чтобы не работать по указателям)
//  for (register u8 i = 0; i < 16; i++)
//  {
//    temp <<= 1;
//    while (LINE_IS_0);        // Ждем пока диния не станет в "1"
//    OWI_delay_us(20);
//    if (LINE_IS_1) { temp++; }
//    while (LINE_IS_1);        // Ждем пока диния не станет в "0"
//  }
//  return temp;
//}


void OWI_CMD_Data16(u8 CMD, u16 data)
{
  OWI_L;  OWI_delay_us(20);       // Start OWI
  OWI_writeByte(SLAVE_ADDR_W);    // Send DeviceSlave Address WRITE
  OWI_writeByte(CMD);             // CMD send
  OWI_writeWord(data);            // write data 16 bits
  OWI_H;  OWI_delay_us(170);      // Hold time stop condition in HIGH level
}

void OWI_CMD(u8 CMD)
{
  OWI_L;  OWI_delay_us(20);       // Start OWI
  OWI_writeByte(SLAVE_ADDR_W);    // Send DeviceSlave Address WRITE
  OWI_writeByte(CMD);             // CMD send
  OWI_H;  OWI_delay_us(170);      // Hold time stop condition in HIGH level
}
void Test()
{
  fp=fopen(GPIO_PIN_PATH_OWI"/value","rw");
  u8 buf[10];
  fread(buf, 1,1, fp);
  //u8 bytes_read = read(fp, buf, 1);
  //printf(outtt);
  qDebug() << buf[0];
  fclose(fp);
}
//void OWI_readData(u16* data)
//{
//  OWI_L;  OWI_delay_us(20);       // Start OWI
//  OWI_writeByte(SLAVE_ADDR_R);    // Send DeviceSlave Address READ
//  SET_LINE_INPUT;                 // Меняем выход на вход (подтяжка вверх LL_GPIO_PULL_UP)
//  *data = OWI_readWord();         // read 16 bits
//  SET_LINE_OUTPUT;                // Меняем вход на выход
//  OWI_delay_us(170);  OWI_H;      // Hold time stop condition is LOW level
//}
//
//u16 OWI_getReg(u8 reg, u8 memType)
//{
//  u16 regValue;
//  OWI_CMD(reg + memType);
//  OWI_readData(&regValue);
//  return regValue;
//}
