#ifndef SPITHREAD_H
#define SPITHREAD_H

#include <QThread>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t

#define GPIO_PIN_PATH_CS "/sys/class/gpio/gpio26"   // OWI
#define SLAVE_ADDR_R 0xF1
#define CS_L()\
{ fpCS=fopen(GPIO_PIN_PATH_CS"/value","w");\
  fprintf(fpCS,"0");\
  fclose(fpCS); }

#define CS_H()\
{ fpCS=fopen(GPIO_PIN_PATH_CS"/value","w");\
  fprintf(fpCS,"1");\
  fclose(fpCS); }

class spiThread : public QThread
{
    Q_OBJECT
  public:
    explicit spiThread(int ptrSpiDevice);
    //spiThread *mSpiThread {nullptr};
    u16 rawData;
    void run();
    bool isStopped;
    bool isRunning;
  private:
    int mFd;
    FILE *fpCS = nullptr;
};

#endif // SPITHREAD_H
