#ifndef FILEIMITSENSOR_H
#define FILEIMITSENSOR_H

#include <QObject>
#include <QTimer>
#include <QFile>

#include "sensor/isensor.h"

class FileImitSensor : public ISensor
{
    Q_OBJECT

    /*! Карта всех свойств */
    QVariantMap mPropertyMap;

    /*! Таймер для запроса показаний */
    QTimer *mDataRequestTimer {nullptr};
    /*! Кол-во вычитывания из файла показаний датчика в секунду */
    int mReadPerSec {400};//{50};//

    /*! Данные из вычитанного файла */
    QVector<float> mSensorReadings;
    /*! Для бесконечного вычитывания mTestData */
    uint mTimeIter = 0;

    /*! Время последнего сброса датчика */
    int64_t mLastResetTimestamp {0};

    /*! Последнее показание, снятое с датчика */
    ComplexValue mLastReading {};
public:
    /*! Текущий статус датчика */
    SensorStatus mStatus {SensorStatus::WaitInited};
private:
    /*! Файл считан */
    bool mFileIsRead {false};

public:
    /*! Свойство - путь к файлу */
    static const QString FilePathProperty;

public:
    FileImitSensor();

    // ISensor interface
public:
    ~FileImitSensor() override {};

    /*! Инициализация имитации датчика
        Если какие-то свойства были инициализированы раньше
        и требуется замена их значение, необходимо вызвать
        deinitialize для полного сброса датчика.
        args - мапа со свойствами и их значениями.
    */
    bool init(const QVariantMap &args) override;
    bool isInit() const override;
    bool config(const QString &jsonFilePath) override;
    bool isConfig() const override;
    bool startSendingSensorReadings() override;
    bool isStartedSendingSensorReadings() const override;
    bool isSensorConnected() const override;
    void endSendingSensorReadings() override;
    void hardReset() override;
    bool needMakeSensorReset() const override;
    bool canMakeSensorReset() const override;
    bool makeSensorReset() override;
    int64_t lastResetTimestamp() const override;
    ComplexValue readData() const override;
    SensorStatus status() const override;

    bool isAllPropertiesToInitAreSet() const override;
    bool hasSensorProperty(const QString &key) const override;
    QVariant sensorProperty(const QString &key) const override;
    QVariantMap sensorProperties() const override;
    /*! Устанавливает свойство
        Если свойство было установлено ранее, оно будет проигнорировано
    */
    bool setSensorProperty(const QString &key, const QVariant &value) override;
    /*! Устанавливает свойства
        Если какие-то свойства были установлены ранее, они будут проигнорированы
    */
    void setSensorProperties(const QVariantMap &properties) override;
    bool propertyHasBeenSet(const QString &key) const override;
    void resetPropertyMap() override;
    QString info() const override;

private:
    /*! Считывание всех показаний из файла */
    bool readAllSensorReadingsFromFile();

protected slots:
    /*! Проверка подключения датчика */
    void checkSensorConnection() override;

private slots:
    /*! Запрос поазаний с датчика */
    void dataRequest() override;

    // ISensor interface
public:
    void softReset() override;
};

#endif // FILEIMITSENSOR_H
