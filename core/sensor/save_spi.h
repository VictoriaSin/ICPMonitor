#ifndef SAVE_SPI_H
#define SAVE_SPI_H

#include <QObject>
#include <QFile>
#include <QThread>
#include "../app/global_define.h"

class CurrentGraphsArea;

class SaveSPI : public QThread
{
    Q_OBJECT

public:
    SaveSPI();
    ~SaveSPI();
    volatile bool isStopped;
    volatile bool isRunning;
    _mSPIData currMesuring;
    volatile bool isRecording;
    volatile qint64 startTime;
    QString fileName = "";
protected:
    void run();
    QFile* fileForSave {nullptr};
};

#endif // SAVE_SPI_H
