#include "averageicpcontroller.h"
#include "controller/settings.h"

#include <QDateTime>
#include <QTimer>
#include <QDebug>

AverageICPController::AverageICPController(Settings *settings,
                                           QObject *parent)
    : QObject{parent}
    , mSettings(settings)
    , mAverageTimer(new QTimer(this))
{
    mAverageTimer->setInterval(settings->getConstIntervalBetweenAverageSensorReadingsMs());

    //connect(mAverageTimer, &QTimer::timeout, this, &AverageICPController::recalcAverage);
}

void AverageICPController::start()
{
    mAverageTimer->start();
}

void AverageICPController::stop()
{
    mData.clear();
    mAverageTimer->stop();
}

const ComplexValue &AverageICPController::lastAverageValue() const
{
    return mLastAverageValue;
}

ComplexValue *AverageICPController::lastAverageValueByPointer()
{
    return &mLastAverageValue;
}

void AverageICPController::setSensorData(const ComplexValue &sensorData)
{
    if(sensorData.valid && mAverageTimer->isActive())
    {

#ifdef QT_DEBUG
        mDifTime = sensorData.timestamp - mLastTimeStamp;
        if (mDifTime != 100) {
            mCountMiss += 1;
        }
        mLastTimeStamp = sensorData.timestamp;
#endif
        mData.append(sensorData.value);
    }
}

void AverageICPController::terminate()
{
    stop();
}

void AverageICPController::recalcAverage()
{
    const int dataCount = mData.size();
    if(dataCount <= 0)
    {
        return;
    }
    ComplexValue average;

    for(const auto &value : qAsConst(mData))
    {
        average.value += value;
    }
    average.value /= dataCount;

#ifdef QT_DEBUG
    //qDebug() << "Miss data: " << mCountMiss;
    mCountMiss = 0;
#endif

    mData.clear();

    average.valid = true;
    average.timestamp = QDateTime::currentMSecsSinceEpoch();

    mLastAverageValue = average;
    emit dataReady();

    qDebug() << "Average value:" << mLastAverageValue.value << mLastAverageValue.timestamp << dataCount;
}
