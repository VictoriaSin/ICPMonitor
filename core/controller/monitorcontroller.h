#ifndef MONITORCONTROLLER_H
#define MONITORCONTROLLER_H

#include <memory>

#include <QObject>
#include <QDir>
#include <QLocale>

#include "controller_enums.h"
#include "global_enums.h"
#include "controller/alarm_enums.h"
#include "controller/fileProcessing/export_enums.h"
#include "blockDevices/blockdevicemanager_enums.h"
#include "global_structs.h"

class Settings;
class AlarmController;
class BlockDeviceManager;
class BlockDevice;
class AverageICPController;
class LabelManager;
class Label;

extern LabelManager *mLabelManagerGlobal;

class MonitorController : public QObject
{
    Q_OBJECT

    /*! Состояния системы */
    enum class State {
        InitSoftwareStorage = 3,
    };
public:
    AverageICPController *mAverageICPController {nullptr};
    AlarmController *mAlarmController {nullptr};
    /*! Состояние базы данных приложения */
    ControllerEvent mDataBaseState {ControllerEvent::DataBaseUnavailable};

    /*! Программное хранилище */
    std::shared_ptr<BlockDevice> mSoftwareStorage;

    /*! Состояние программного хранилища */
    ControllerEvent mSoftwareStorageState {ControllerEvent::SoftwareStorageUnavailable};
public:    
    explicit MonitorController();
    /*! Возвращает состояние программного хранилища */
    ControllerEvent getSoftwareStorageState() const;
    /*! Установка програмнного хранилища */
    void setSoftwareStorage(const std::shared_ptr<BlockDevice> &devStorage);
    /*! Программное хранилище */
    std::shared_ptr<BlockDevice> getSoftwareStorage() const;
    /*! Возвращает все доступные блочные устройства */
    //QList<std::shared_ptr<BlockDevice>> getAvailableBlockDevices() const;
    void terminate();
    Settings *settings() const;
    /*! Установка языка всего приложения */
    bool setAppLanguage(QLocale::Language language);
    /*! Создаёт пользовательскую метку */
    void makeLabel();
    /*! Возвращает кол-во сделанных меток за сессию */
    int getLabelsCountPerCurrentSession() const;
    /*! Возвращает менеджера меток */
    LabelManager *getLabelManager() const;

    /*################################################################################
                                        Тревога
    ################################################################################*/
    /*! Установка диапазона допустимых значений, при которых не срабатывает тревога, и
        установка состояний тревоги для верхней и нижней границ (включена/выключена).
     */
    bool setLevelsAndStatesAlarm(int lowLevelAlarm, int highLevelAlarm,
                                 bool lowEnabel, bool highEnable);
    /*################################################################################
                                       Масштаб
    ################################################################################*/
    bool setGeneralParam(float mFontScaleFactor);
    /*################################################################################
                                        График
    ################################################################################*/
    bool setInetrvalsOnGraph(float mCurrentReadingsGraphIntervalX, float mCurrentReadingsGraphIntervalY,
                             float mCurrentReadingsGraphIntervalYHigh, float mTickCountX, float mTickCountY);
    bool setPressureUnits(uint8_t mCurrentPressureUnitsIndex);
    bool setAverageInterval(float mAverageIntervalSec);
    void deleteLabel();
private:
    /*! Программное хранилище стало доступно */
    void softwareStorageUnavailable();

private slots:
    void processAlarmEvent(AlarmEvent event, const QVariantMap &args = {});

signals:
    void destroyThread();
    void controllerEvent(ControllerEvent event, const QVariantMap &args = {});
    void dataReadyFromAverageICPController();
};

#endif // MONITORCONTROLLER_H
