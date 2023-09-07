#include "alarmcontroller.h"
#include "controller/settings.h"
#include "controller/averageicpcontroller.h"

#include <QDebug>

AlarmController::AlarmController(Settings *settings, AverageICPController *averageICPControoler,
                                 QObject *parent)
    : QObject{parent}
    , mSettings(settings)
    , mAverageICPController(averageICPControoler)
{
    connect(mAverageICPController, &AverageICPController::dataReady, this, &AlarmController::setSensorAverageData);
}

void AlarmController::setSensorAverageData()
{
    mLastSensorAverageValue = mAverageICPController->lastAverageValue();
    setICPState();
}

void AlarmController::terminate()
{

}

AlarmEvent AlarmController::currentStateAlarm() const
{
    return mCurrentStateAlarm;
}

void AlarmController::clear()
{
    mLastSensorAverageValue.clear();
    mCurrentStateAlarm = AlarmEvent::None;
    mPreviousStateAlarm = AlarmEvent::None;
}

void AlarmController::setICPState()
{
    if (mLastSensorAverageValue.valid) {
        // Если текущее среднее значение меньше нижней границы сигнализации, выставленной пользователем и сигнализация включена
        if (mLastSensorAverageValue.value < mSettings->getLowLevelAlarm() && mSettings->getLowLevelStateAlarm()){
            mCurrentStateAlarm = AlarmEvent::Low;
        // Если текущее среднее значение выше верхней границы сигнализации, выставленной пользователем и сигнализация включена
        } else if (mLastSensorAverageValue.value > mSettings->getHighLevelAlarm() && mSettings->getHighLevelStateAlarm())
        {
            mCurrentStateAlarm = AlarmEvent::High;
        } else
        {
            mCurrentStateAlarm = AlarmEvent::Normal;
        }
    } else {
        mCurrentStateAlarm = AlarmEvent::None;
    }

    if (mPreviousStateAlarm != mCurrentStateAlarm) {
        mPreviousStateAlarm = mCurrentStateAlarm;
        emit alarmEvent(mCurrentStateAlarm);
    }
}
