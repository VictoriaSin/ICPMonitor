#ifndef AVERAGEICPCONTROLLER_H
#define AVERAGEICPCONTROLLER_H

#include <QObject>

#include "global_structs.h"

class Settings;
class QTimer;

class AverageICPController : public QObject
{
    Q_OBJECT

public:
    Settings *mSettings {nullptr};
    QTimer *mAverageTimer {nullptr};

    ComplexValue mLastAverageValue;
    QVector<float> mData;
#ifdef QT_DEBUG
    int64_t mDifTime {0};
    int mCountMiss {0};
    int64_t mLastTimeStamp {0};
#endif


    AverageICPController(Settings *settings, QObject *parent = nullptr);
    ~AverageICPController();


    /*! Возвращает последнее вычисленное среднее значение */
    const ComplexValue &lastAverageValue() const;
    ComplexValue *lastAverageValueByPointer();
    void setSensorData(const ComplexValue &sensorData);
    void terminate();

signals:
    /*! Отправляется каждый раз, когда новые данные доступны для чтения */
    void dataReady();    

private slots:
    void recalcAverage();
public slots:
    void start();
    void stop();
};

#endif // AVERAGEICPCONTROLLER_H
