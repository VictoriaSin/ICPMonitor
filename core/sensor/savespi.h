#ifndef SAVESPI_H
#define SAVESPI_H

#include <QObject>
#include <QFile>
#include <QThread>
#include "savespi_1.h"

class CurrentGraphsArea;

class SaveSPI : public QThread
{
    Q_OBJECT

public:
    SaveSPI();
    ~SaveSPI();

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

    SaveSPI_1 *mSaveSPI_1 = nullptr;
    void run();
    uint16_t calcAverage(uint16_t data);
};

#endif // SAVESPI_H
