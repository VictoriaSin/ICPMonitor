#ifndef READ_SPI_H
#define READ_SPI_H

#include <QObject>
#include <QFile>
#include <QThread>
#include "save_spi.h"

class CurrentGraphsArea;

class ReadSPI : public QThread
{
    Q_OBJECT

public:
    ReadSPI();
    ~ReadSPI();

    volatile bool isStopped;
    volatile bool isRunning;
    volatile bool isRecording;
protected:
    float AverageIntervalSec{1.5};
    uint maxBuffSizeAvg;
    uint16_t firstBuffPointer;
    uint16_t lastBuffPointer;
    uint sum;//double sum;
    uint16_t cnt;
    uint16_t *CurrDataForAverage{nullptr};

    SaveSPI *mSaveSPI = nullptr;
    void run();
    uint16_t calcAverage(uint16_t data);
};

#endif // READ_SPI_H
