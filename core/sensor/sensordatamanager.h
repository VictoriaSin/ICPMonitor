#ifndef SENSORDATAMANAGER_H
#define SENSORDATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QDir>
#include <QFile>



class SensorDataManager : public QThread
{
    Q_OBJECT
    volatile qint64 timerForRecordInFile, timerForSetInGraph;
public:
    explicit SensorDataManager(QObject *parent = nullptr, QString dirPath = "");
    SensorDataManager();
    ~SensorDataManager();
    volatile bool isStopped;
    volatile bool isRunning;
    volatile bool isStopSensorData;
    volatile bool isRecording;
    void startRecord();
protected:
    void run();
    uint16_t first;
    uint16_t last;
    double sum;
    uint16_t cnt;
    double *CurrDataForAverage;
    double calcAverage(int data);
    volatile int32_t currIndex;
    uint32_t currIndexDrawGraph;

    QFile SPIData;

signals:
    void writeBufferToFile();
    void printDataOnGraph(unsigned int, unsigned int);
    void averageReady(double currAverage);

public slots:
    //void stopReading();
    //void addDataToBuffer();
private:
    double mAverageValue;
    double mAverageSum;
    uint64_t mAverageCount;
    float AverageIntervalSec{1.5};
    int buffSize;
};

#endif // SENSORDATAMANAGER_H
