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

    bool isStartRecord;
    bool isRecording;
protected:
    void run();

signals:
    void writeBufferToFile();
    void printDataOnGraph(unsigned int, unsigned int);//uint32_t x, uint32_t y);
    void averageReady(double currAverage);

public slots:
    //void stopReading();
    //void addDataToBuffer();
private:
    double mAverageValue;
    double mAverageSum;
    uint64_t mAverageCount;
};

#endif // SENSORDATAMANAGER_H
