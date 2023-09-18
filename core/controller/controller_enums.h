#ifndef CONTROLLER_ENUMS_H
#define CONTROLLER_ENUMS_H

#include <QObject>
#include <QVariant>

namespace ControllerEventsArgs {
    constexpr static char LabelNumber[] = "LN";
    constexpr static char LabelCreationTimeMs[] = "LCT";
}

/*! События Контроллера

    Множество возможных событий включает:
    - события от Сенсора
    - события от Тревоги
    - события от Экспорта файлов
 */
enum class ControllerEvent {
    SensorInited,               ///< Датчик инициализирован (АЦП)
    SensorInitError,            ///< Ошибка инициализации датчика (АЦП)

    SensorConnected,            ///< Датчик подключен
    SensorConnectionError,      ///< Ошибка подключения к датчику

    SensorConfigured,           ///< Датчик конфигурируется
    SensorConfigError,          ///< Ошибка конфигурации датчика

    SensorStartDispatchSensorReadings,///< Датчик начал отправлять показания
    SensorStopDispatchSensorReadings, ///< Датчик закончил отправлять показания

    SensorDisconnected,         ///< Датчик отключен физически

    SensorReset,                ///< Датчик обнулен
    SensorResetError,           ///< Обнуление не выполнено из-за ошибки

    SensorHardReseted,          ///< Датчик был полностью сброшен

    SensorUndefinedError,       ///< Иная ошибка датчика

    AlarmLowOn,                 ///< Сработала сигнализация по нижней границе
    AlarmHighOn,                ///< Сработала сигнализация по верхней границе
    AlarmOff,                   ///< Сигнализация отключена

    GlobalTimeUpdate,           ///< Системное время обновилось

    SessionStarted,             ///< Сессия началась
    SessionClosed,              ///< Сессия закончилась

    /*! Прогресс экспорта
        Передаваемые аргументы:
        "Transferred fileName" (QString) - имя текущего экспортируемого файл;
        "Transferred files" (int) - кол-во экспортированных файлов;
        "Count files" (int) - общее кол-во экспортируемых файлов;
     */
    ExportProgress,
    ///< Ошибка экспорта
    ExportError,
    /*! Экспорт завершён
        Передаваемые аргументы:
        "Data type" (QString) - экспортируемые данные (Например: Изображения, Средние значения датчика и тд.);
        "Transferred files" (int) - кол-во экспортированных файлов;
        "Count files" (int) - общее кол-во экспортируемых файлов;
        "Untransferred files" (QStringList) - имена не переданных файлов.
     */
    ExportDone,

    LabelCreated,               ///< Метка создана (Используется с controllereventsargs::LabelNumber и номером созданной метки)

    BlockDeviceConnected,           ///< Подключено блочное устройство
    BlockDeviceDisconnected,        ///< Отключено блочное устройство
    BlockDeviceChanged,             ///< Изменения с блочным устройством

    SoftwareStorageAvailable,           ///< Программное хранилище доступно
    SoftwareStorageUnavailable,         ///< Программное хранилище не доступно
    SoftwareStorageNotAssigned,         ///< Программное хранилище не назначено

    DataBaseAvailable,                  ///< База данных доступна
    DataBaseUnavailable,                ///< База данных не доступна

    NewSoftwareStorageError,            ///< Новое программное хранилище не установлено

    ScreenWritten,
    ScreenNotWritten,

    UpdatedMaxScreens,                  ///< Обновлено максимальное кол-во скриншотов

    UpdatedAlarmLevels,                 ///< Обновлены уровни тревоги
    UpdatedAlarmStates,                 ///< Обновлены состояния тревоги

    RecordCurrentSensorReadingsInterruptedError, ///< Запись текущих показаний прервана ошибкой
    RecordAverageSensorReadingsInterruptedError,  ///< Запись средних показаний прервана ошибкой

    UpdateGraphIntervals,                ///< Обновлены интервалы осей
    UpdateGraphTicks,                    ///< Обновлены деления осей

    ChangePressureUnits,                 ///< Изменены единицы измерения давления

    UpdateGeneralGroupInfo               ///< Обновлены параметры данной группы .ini файла
};

Q_DECLARE_METATYPE(ControllerEvent);

#endif // CONTROLLER_ENUMS_H
