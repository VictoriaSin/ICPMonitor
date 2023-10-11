#ifndef ALARMCONTROLLER_H
#define ALARMCONTROLLER_H

#include <QObject>
#include <QVariant>

#include "global_structs.h"
#include "controller/alarm_enums.h"

class Settings;
class AverageICPController;

class AlarmController : public QObject
{
    Q_OBJECT

    Settings *mSettings {nullptr};
    AverageICPController *mAverageICPController {nullptr};

    ComplexValue mLastSensorAverageValue;

    AlarmEvent mCurrentStateAlarm {AlarmEvent::None};
    AlarmEvent mPreviousStateAlarm {AlarmEvent::None};

public:
    explicit AlarmController(Settings *settings, AverageICPController *averageICPControoler, QObject *parent = nullptr);

    void setSensorAverageData();



    AlarmEvent currentStateAlarm() const;

    /*! Отчистка контроллера тревоги */
    void clear();

private:
    void setICPState();

signals:
    void alarmEvent(AlarmEvent event, const QVariantMap &args = {});
};

#endif // ALARMCONTROLLER_H
