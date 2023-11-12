#ifndef SPITHREAD_H
#define SPITHREAD_H

#include <QThread>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t

class spiThread : public QThread
{
    Q_OBJECT
  public:
    explicit spiThread(int ptrSpiDevice);
    volatile u16 rawData;
    void run();
    volatile bool isStopped;
    volatile bool isRunning;
  private:
    int mFd;
};

#endif // SPITHREAD_H
