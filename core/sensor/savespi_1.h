#ifndef SAVESPI_1_H
#define SAVESPI_1_H

#include <QObject>
#include <QFile>
#include <QThread>
#include "../app/global_define.h"

class CurrentGraphsArea;

class SaveSPI_1 : public QThread
{
    Q_OBJECT

public:
    SaveSPI_1();
    ~SaveSPI_1();
    volatile bool isStopped;
    volatile bool isRunning;
    _mSPIData temp;
    volatile bool isRecording;
protected:
    void run();
};

#endif // SAVESPI_1_H
