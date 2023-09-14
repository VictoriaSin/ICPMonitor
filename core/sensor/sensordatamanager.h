#ifndef SENSORDATAMANAGER_H
#define SENSORDATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QThread>

class SensorDataManager : public QThread//public QObject
{
    Q_OBJECT
    uint32_t currIndex;
    uint64_t msCount;
    QTimer *getSPITimer;
public:
    SensorDataManager();
    ~SensorDataManager();
protected:
    bool readDataFromSensor();
    bool addDataToBuffer(uint64_t data);
    void run();
signals:
    void writeBufferToFile();
    void printDataOnGraph(uint64_t x, uint64_t y);
public slots:
    void stopReading();
};

#endif // SENSORDATAMANAGER_H
