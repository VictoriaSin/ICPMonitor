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
    SaveSPI_1 *mSaveSPI_1 = nullptr;
    void run();
};

#endif // SAVESPI_H
