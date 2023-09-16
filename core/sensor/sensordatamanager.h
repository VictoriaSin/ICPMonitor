#ifndef SENSORDATAMANAGER_H
#define SENSORDATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QThread>



class SensorDataManager : public QThread//public QObject
{
    Q_OBJECT
    volatile qint64 timerForRecordInFile, timerForSetInGraph;
public:
    explicit SensorDataManager(QObject *parent = nullptr);
    SensorDataManager();
    ~SensorDataManager();
    volatile bool isStopped;
    volatile bool isRunning;

protected:
    void run();

signals:
    void writeBufferToFile();
    void printDataOnGraph(unsigned int, unsigned int);//uint32_t x, uint32_t y);

public slots:
    //void stopReading();
    //void addDataToBuffer();
public:

};

#endif // SENSORDATAMANAGER_H
