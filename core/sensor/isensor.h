#ifndef ISENSOR_H
#define ISENSOR_H

#include <QObject>
#include <QVariant>

#include "global_structs.h"
#include "sensor_enums.h"

/*! Интерфейс сенсора

    В частности, этот интерфейс может быть реализован
    как сенсор ICP - данные давления,
    или как имитатор данных давления
 */
class ISensor : public QObject
{
    Q_OBJECT

public:
    virtual ~ISensor() {};

    /*! Инициализация сенсора произвольными данным,
        заданными в виде пары (ключ, значение)
     */
    virtual bool init(const QVariantMap &/*args*/) = 0;

    /*! Был ли проинициализирован датчик */
    virtual bool isInit() const = 0;

    /*! Конфигурация датчика из файла json */
    virtual bool config(const QString &jsonFilePath) = 0;

    /*! Был ли сконфигурирован датчик */
    virtual bool isConfig() const = 0;

    /*! Начать рассылку показаний датчика */
    virtual bool startSendingSensorReadings() = 0;

    /*! Начата ли рассылка показаний датчика */
    virtual bool isStartedSendingSensorReadings() const = 0;

    /*! Подключён ли датчик */
    virtual bool isSensorConnected() const = 0;

    /*! Прекращение рассылки показаний датчика */
    virtual void endSendingSensorReadings() = 0;

    /*! Полный сброс к начальному состоянию */
    virtual void hardReset() = 0;

    /*! Не полный сброс датчика */
    virtual void softReset() = 0;

    /*! Проверка необходимости выполнения обнуления сенсора
        Возвращаемый результат может быть расширен
        до указания конкретных аспектов через отдельный enum
        по необходимости
     */
    virtual bool needMakeSensorReset() const = 0;

    /*! Проверка возможности выполнения обнуления сенсора
        Возвращаемый результат может быть расширен
        до указания конкретных аспектов через отдельный enum
        по необходимости
     */
    virtual bool canMakeSensorReset() const = 0;

    /*! Выполнение обнуления сенсора
        Возвращаемый результат может быть расширен
        до указания конкретных аспектов через отдельный enum
        по необходимости
     */
    virtual bool makeSensorReset() = 0;

    /*! Время выполнения последнего обнуления, ms
        Если 0, то обнуление не выполнялось.
     */
    virtual int64_t lastResetTimestamp() const = 0;

    /*! Возвращает последние считанные данные */
    virtual ComplexValue readData() const = 0;

    /*! Статус сенсора/соединения */
    virtual SensorStatus status() const = 0;

    /*! Установлены ли все свойства для инициализации */
    virtual bool isAllPropertiesToInitAreSet() const = 0;

    /*! Выполняет проверку, возможно ли прочитать данное свойство по ключу */
    virtual bool hasSensorProperty(const QString &/*key*/) const = 0;

    /*! Вернуть значение свойства по ключу key */
    virtual QVariant sensorProperty(const QString &/*key*/) const = 0;

    /*! Вернуть значения всех свойства сенсора */
    virtual QVariantMap sensorProperties() const = 0;

    /*! Установка значения свойства сенсора value по ключу key */
    virtual bool setSensorProperty(const QString &key, const QVariant &value) = 0;

    /*! Установка свойств датчика */
    virtual void setSensorProperties(const QVariantMap &properties) = 0;

    /*! Установлено ли свойство */
    virtual bool propertyHasBeenSet(const QString &key) const = 0;

    /*! Обнуление всех свойств */
    virtual void resetPropertyMap() = 0;

    /*! Строка-базовая информация о сенсоре.
        Не обязательно должна быть уникальной для каждого сенсора.
        Служит для наглядного понимания того, что это за реализация
        Это не id сенсора!
        По необходимости метод получения id сенсора может быть добавлен в интерфейс
     */
    virtual QString info() const = 0;

protected slots:
    /*! Проверка подключения датчика */
    virtual void checkSensorConnection() = 0;

    /*! Запрос данных датчика */
    virtual void dataRequest() = 0;

signals:
    /*! Отправляется каждый раз, когда новые данные доступны для чтения */
    void dataReady();

    void sensorEvent(SensorEvent event, const QVariantMap &args = {});
};

#endif // ISENSOR_H
