#ifndef MONITORCONTROLLER_H
#define MONITORCONTROLLER_H

#include <memory>

#include <QObject>
#include <QDir>
#include <QLocale>

#include "controller_enums.h"
#include "sensor/sensor_enums.h"
#include "global_enums.h"
#include "controller/alarm_enums.h"
#include "controller/fileProcessing/filecontroller_events.h"
#include "controller/fileProcessing/export_enums.h"
#include "blockDevices/blockdevicemanager_enums.h"
#include "global_structs.h"

class Settings;
class AlarmController;
class FileController;
class BlockDeviceManager;
class BlockDevice;
class AverageICPController;
class OutputICPSerializer;
class SessionManager;
class LabelManager;
class DataBaseManager;
class Session;
class Label;

class ISensor;

extern LabelManager *mLabelManagerGlobal;

class MonitorController : public QObject
{
    Q_OBJECT

    /*! Состояния системы */
    enum class State {
        InitSystem = 0,
        DeinitSystem = 1,
        ReinitSystem = 2,
        InitSoftwareStorage = 3,
        DeinitSoftwareStorage = 4,
        UpdateSoftwareStorage = 5,
        InitDataBase = 6,
        DeinitDataBase = 7,
        ReinitDataBase = 8,
        UpdateDataBase = 9,
        UpdateFileController = 10,
        InitRTCModule = 11,
        InitSensor = 12,
    };
public:
    Settings *mICPSettings {nullptr};

    AverageICPController *mAverageICPController {nullptr};
    AlarmController *mAlarmController {nullptr};
    FileController *mFileController {nullptr};
    BlockDeviceManager *mBlockDeviceManager {nullptr};

    QThread *mSensorThread {nullptr};
    ISensor *mSensor {nullptr};

    /*! Поток для менеджера блочных устройств и сохранения скриншотов */
    QThread *mWorkerThread {nullptr};

    /*! Последнее показание датчика */
    ComplexValue mLastSensorValue {};

    /*! Последнее сконвертированое показание датчика
        Сконвертировано в зависимости
        от выбора пользователя
    */
    ComplexValue mLastConvertedSensorValue {};

    /*! Менеджер сессий */
    SessionManager *mSessionManager {nullptr};

    /*! База данных приложения */
    DataBaseManager* mDataBaseManager {nullptr};

    /*! Состояние базы данных приложения */
    ControllerEvent mDataBaseState {ControllerEvent::DataBaseUnavailable};

    /*! Кол-во попыток смонтировать программное хранилище */
    const uint8_t MaxTryOpenDataBase {5};

    /*! Текущее кол-во попыток смонтировать программное хранилище */
    uint8_t mCurrentNumOpenDataBase {0};

    /*! Интервал попыток открытия базы данных */
    uint16_t mIntervalOpenDataBaseMs {100};

    ///*! Менеджер меток */
    //LabelManager *mLabelManager {nullptr};

    /*! Программное хранилище */
    std::shared_ptr<BlockDevice> mSoftwareStorage;

    /*! Состояние программного хранилища */
    ControllerEvent mSoftwareStorageState {ControllerEvent::SoftwareStorageUnavailable};

    /*! Кол-во попыток смонтировать программное хранилище */
    const uint8_t MaxTryMountSoftwareStorage {5};

    /*! Текущее кол-во попыток смонтировать программное хранилище */
    uint8_t mCurrentNumMountSoftwareStorage {0};

    /*! Интервал попыток монтирования программного хранилища */
    uint16_t mIntervalMountSoftwareStorageMs {250};

public slots:
    /*! Деинициализация контроллера */
    void deinit();
//signals:
//    void stopAverageTimer();
public:    
    explicit MonitorController();

    /*! Инициализация контроллера */
    void init();

//    /*! Деинициализация контроллера */
//    void deinit();

    /*! Возвращает состояние программного хранилища */
    ControllerEvent getSoftwareStorageState() const;

    /*! Установка програмнного хранилища */
    void setSoftwareStorage(const std::shared_ptr<BlockDevice> &devStorage);

    /*! Программное хранилище */
    std::shared_ptr<BlockDevice> getSoftwareStorage() const;

    /*! Возвращает все доступные блочные устройства */
    QList<std::shared_ptr<BlockDevice>> getAvailableBlockDevices() const;

    /*! Инициализация датчика */
    void initSensor();

    void terminate();

    Settings *settings() const;

    ComplexValue getLastAverageValue() const;

    /*! Установка языка всего приложения */
    bool setAppLanguage(QLocale::Language language);

    /*! Создаёт пользовательскую метку */
    void makeLabel();

    /*! Возвращает кол-во сделанных меток за сессию */
    int getLabelsCountPerCurrentSession() const;

    /*! Возвращает текущую сессию
        Возвращает пустой shared_ptr, если сессии нет.
    */
    std::shared_ptr<Session> getCurrentSession() const;

    /*! Возвращает все метки, принадлежащие указанной сессии */
    QVector<std::shared_ptr<Label>> getAllLabelsBelongSession(int sessionID);

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
    bool setInetrvalsOnGraph(double mCurrentReadingsGraphIntervalX, double mCurrentReadingsGraphIntervalY,
                             double mTickCountX, double mTickCountY);
    /*################################################################################
                                        Время
    ################################################################################*/
    /*! Сохранение текущего времени системы */
    void saveCurrentDateTime();

    /*! Установка текущего времени и даты */
    void setDateTime(int64_t timestamp);

    /*! Проверка на корректность даты.
        Сравнивается с Settings->MinDateTime (минимальной валидной датой)
     */
    bool dateTimeIsValid(const QDateTime &checkDate) const;

    /*! Проверка на корректность текущей даты.
        Сравнивается с Settings::MinDateTime (минимальной валидной датой)
     */
    bool currentTimeIsValid() const;
    /*################################################################################
                                      Изображения
    ################################################################################*/
    /*! Запись скриншота из Linux Frame Buffer (/dev/fb0)
        Будет работать только если видеокарта рисует в него
    */
    void writeScreenFromLinuxFB();

    /*! Запись скриншота из виджета */
    void writeScreenFromWidget(const QPixmap &screen);

    /*! Установка максимального кол-ва скриншотов */
    void setMaxScreens(uint maxScreens);

    /*! Загружает указанное кол-во скриншотов с устройства на флешку.
        Если указанное кол-во фото больше, чем находится на устройства,
        то загружаются все фотографии с устройства.
        Максимальное кол-во фотографий задано в Setting - mMaxPictures.

        Параметры:
        format - формат конвертирования изображения
        countTransScreenshoots - количество фотографий, необходимых для экспорта
    */
    void exportScreens(ConversionPictureFormat format, int countTransScreens);
    /*################################################################################
                                Текущие показания датчика
    ################################################################################*/
    /*! Экспорт текущих показаний датчика
        exportTimeMs - время необходимого экспорта в миллисекундах;
        exportValues - значения, необходимые для экспорта;
        dateConverterFunc - функция для конвертации даты.
    */
    void exportLastAbsoluteTimeCurrentSensorReadings(uint64_t exportTimeMs, uint32_t exportValues, ExportDataFunc func);
    /*################################################################################
                                Средние показания датчика
    ################################################################################*/
    /*! Экспорт средних показаний датчика
        exportTimeSec - время необходимого экспорта в секундах;
        exportValues - значения, необходимые для экспорта;
        dateConverterFunc - функция для конвертации даты.
    */
    void exportLastAbsoluteTimeAverageSensorReadings(uint64_t exportTimeSec, uint32_t exportValues, ExportDataFunc func);

    /*! Возвращает отсортированный по увеличению времени контейнер средних значений, попадающих в указанный интервал
        intervalStartTimeSeconds - начало интервала средних значений;
        intervalEndTimeSeconds - конец интервала средних значений.
    */
    QVector<ComplexValue> getAverageSensorReadingsFromTheInterval(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds);

    /*! Возвращает отсортированный по увеличению времени контейнер средних значений, попадающих в указанный интервал
        и принадлежащих текущей сессии
        intervalStartTimeSeconds - начало интервала средних значений;
        intervalEndTimeSeconds - конец интервала средних значений.
    */
    QVector<ComplexValue> getAverageSensorReadingsFromTheIntervalForTheCurrentSession(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds);
    /*################################################################################
                                        Датчик
    ################################################################################*/
    /*! Применять только для вызова const (readonly) методов!
        Запрещается связывание сигналов сенсора!
        Обязательно проверять на nullptr!
    */
    const ISensor *sensor() const;

    /*! Возврат последнего показания
        датчика, сконвертированного в
        пользовательские единицы измерения
    */
    const ComplexValue &getLastConvertedSensorValue() const;

    /*! Возврат последнего показания датчика */
    const ComplexValue getLastSensorValue() const;

    /*! Тестовая, очень простая реализация! */
    bool testMakeSensorReset();

    /*! Закрывает сессию */
    void closeSession();
    void deleteLabel();
    QString getFlashDevice(QString pathPartDevice);
private:
    /*! Запуск состояния */
    void runState(State state);

    /*! Инициализация программного хранилища
        Монтирование происходит асинхронно.
        Если оно будет успешно, то
        подключится и БД. Если не успешно,
        то сообщит о недоступности хранилища.
    */
    void initSoftwareStorage();

    /*! Обновление программного хранилища */
    void updateSoftwareStorage();

    /*! Инициализировано ли программное хранилище */
    bool isInitSoftwareStorage();

    /*! Программное хранилище подключилось */
    void softwareStorageConnected();

    /*! Программное хранилище отключилось */
    void softwareStorageDisconnected();

    /*! В программном хранилище произошли изменения */
    void softwareStorageChanged();

    /*! Программное хранилище стало доступно */
    void softwareStorageAvailable();

    /*! Программное хранилище стало доступно */
    void softwareStorageUnavailable();

    /*! Программное хранилище не назначено */
    void softwareStorageNotAssigned();

    /*! Деинициализация программного хранилища */
    void deinitSoftwareStorage();

    /*! Инициализация базы данных */
    void initDataBase();

    /*! Деинициализация базы данных */
    void deinitDataBase();

    /*! Обновление базы данных */
    void updateDataBase();

    /*! База данных доступна */
    void dataBaseAvailable();

    /*! База данных не доступна */
    void dataBaseUnavailable();

    /*! Обновление контроллера файлов */
    void updateFileController();

    /*! Инициализация модуля RTC (реального времени) */
    void initRTCModule();

    /*! Создаёт сессию */
    void createSession();



private slots:
    void processSensorData();
    void processSensorEvent(SensorEvent event, const QVariantMap &args = {});
    void processAlarmEvent(AlarmEvent event, const QVariantMap &args = {});
    void processFileControllerEvent(FileControllerEvent event, const QVariantMap &args = {});
    void processBlockDevicesControllerEvent(BlockDeviceManagerEvent event, const QVariantMap &args = {});
    void processAverageSensorValue();

signals:
    void controllerEvent(ControllerEvent event, const QVariantMap &args = {});
    void dataReadyForGraph();
    void dataReadyFromAverageICPController();
};

#endif // MONITORCONTROLLER_H
