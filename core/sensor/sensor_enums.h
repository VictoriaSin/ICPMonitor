#ifndef SENSOR_ENUMS_H
#define SENSOR_ENUMS_H

#include <QObject>
#include <QMap>

/*! Статус сенсора
    Варианты перечисления по необходимости
    могут быть добавлены
 */
enum class SensorStatus{
    WaitInited,                             ///< Ожидает инициализации
    WaitConnection,                         ///< Ожидает подключения датчика
    WaitConfigured,                         ///< Ожидает конфигурации
    WaitReset,                              ///< Ожидает обнуления
    WaitingStartDispatchSensorReadings,     ///< Ожидает запуска рассылки показаний датчика
    DispatchSensorReadings,                 ///< Рассылка показаний датчика
    Error,                                  ///< Ошибка
};

/*! События Сенсора */
enum class SensorEvent{
    Inited,                     ///< Датчик (АЦП) проинициализирован
    InitError,                  ///< Ошибка инициализации датчика (АЦП)

    Connected,                  ///< Датчик подключён
    ConnectionError,            ///< Ошибка подключения датчика

    Configured,                 ///< Датчик сконфигурирован
    ConfigError,                ///< Ошибка конфигурации датчика

    Reset,                      ///< Датчик обнулен
    ResetError,                 ///< Обнуление не выполнено из-за ошибки

    StartDispatchSensorReadings,///< Рассылка показаний датчика началась
    StopDispatchSensorReadings, ///< Рассылка показаний датчика закончилась

    HardReset,                  ///< Произошёл полный сброс датчика

    Disconnected,               ///< Датчик отключен физически

    UndefinedError              ///< Иная ошибка датчика
};

Q_DECLARE_METATYPE(SensorEvent);


#endif // SENSOR_ENUMS_H
