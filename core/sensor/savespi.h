#ifndef SAVESPI_H
#define SAVESPI_H

#include <QObject>
#include <QFile>
#include <QThread>

class CurrentGraphsArea;

class SaveSPI : public QThread
{
    Q_OBJECT

    //QFile SPIData;
public:
    //SaveSPI(QObject *parent = nullptr, QString dirPath=nullptr);
    SaveSPI();//QObject *parent = nullptr); //, QString dirPath=nullptr);
    ~SaveSPI();
    volatile bool isStopped;
    volatile bool isRunning;
    volatile bool isRecording;
protected:
    void run();
public slots:
    //void fillFile();
};

#endif // SAVESPI_H
