#include "bmp280temperaturespisensor.h"
#include "global_functions.h"
#include "sensor/spi.h"

#include <QDebug>
#include <QDateTime>

const QString BMP280TemperatureSpiSensor::PathToSPIDeviceProperty = "pathToSPIDevice";
const QString BMP280TemperatureSpiSensor::SPISpeedProperty = "SPISpeed";

BMP280TemperatureSpiSensor::BMP280TemperatureSpiSensor()
{
    // Инициализируем карту свойств
    mPropertyMap.insert(PathToSPIDeviceProperty, "");
    mPropertyMap.insert(SPISpeedProperty, "");

    // Настройка таймера генерации показаний
    mDataRequestTimer = new QTimer(this);
    mDataRequestTimer->setInterval(1000 / mReadPerSec);
    connect(mDataRequestTimer, &QTimer::timeout, this, &BMP280TemperatureSpiSensor::dataRequest);
}

BMP280TemperatureSpiSensor::~BMP280TemperatureSpiSensor()
{
    if (mDataRequestTimer) {
        delete mDataRequestTimer;
        mDataRequestTimer = nullptr;
    }

    if (mSPIChannel) {
        delete mSPIChannel;
        mSPIChannel = nullptr;
    }
}

bool BMP280TemperatureSpiSensor::init(const QVariantMap &args)
{
    // Если SPI уже инициализирован
    if (isInit()) {
        return false;
    }

    // Если не все свойства для инициализации установлены
    if (!isAllPropertiesToInitAreSet()) {
        setSensorProperties(args); // Установятся лишь ранее не установленные свойства
    }

    // Установлены ли все свойства
    const bool isSetAllProperties = isAllPropertiesToInitAreSet();
    if (!isSetAllProperties) {
        emit sensorEvent(SensorEvent::InitError, QVariantMap{
                             {GlobalEventsArgs::Message, tr("Попробуйте перезапустить устройство.\n"
                              "При повторных неудачах обратитесь к производителю.")}});
        return false;
    }

    // Инициализируем канал общения SPI и открываем
    mSPIChannel = new SPI(mPropertyMap[PathToSPIDeviceProperty].toString().toStdString(), SPI_MODE_3, mPropertyMap[SPISpeedProperty].toInt());
    mSPIChannel->open();

    // Если SPI канал не открылся
    if (!mSPIChannel->isOpen()) {
        // Увеличиваем кол-во ошибок инициализации
        ++mCountInitError;

        // Удаляем канал SPI
        delete mSPIChannel;

        // Если достигнут предел ошибок инициализации
        if (mCountInitError == MaxCountInitError) {
            qDebug() << "SPI interface not work!";
            emit sensorEvent(SensorEvent::InitError, QVariantMap{
                                 {GlobalEventsArgs::Message, tr("Интерфейс SPI не удалось инициализировать.\n"
                                  "Попробуйте перезапустить устройство.\n"
                                  "При повторных неудачах обратитесь к производителю.")}});
        }

        // Просим переинициализировать канал SPI
        QTimer::singleShot(mReInitMs, this, [this, args](){
            init(args);
        });
    }

    mStatus = SensorStatus::WaitConnection; // Устанавливаем ожидание подключения датчика
    mDataRequestTimer->start(); // Запускаем генерацию запросов данных датчика (Прослушка SPI)
    emit sensorEvent(SensorEvent::Inited);

    return true;
}

bool BMP280TemperatureSpiSensor::isInit() const
{
    return mStatus != SensorStatus::WaitInited;
}

bool BMP280TemperatureSpiSensor::config(const QString &/*jsonFilePath*/)
{
    // Если датчик уже сконфигурирован
    if (isConfig()) {
        return false;
    }

    // Если датчик не проинициализирован или не подключён
    if (!isInit() || !isSensorConnected()) {
        return false;
    }

    // Приостанавливаем опрос датчика
    mDataRequestTimer->stop();

    // Для проверки успешной конфигурации
    bool isGood {true};

    // Конфигурируем датчик
    isGood &= configBMP280();

    // Получаем компенсационные параметры для температуры
    isGood &= takeTemperatureCompensationParametersBMP280();

    // Продолжаем опрашивать датчик
    mDataRequestTimer->start();

    // Если была ошибка конфигурации
    if (!isGood) {
        emit sensorEvent(SensorEvent::ConfigError, QVariantMap{
                             {GlobalEventsArgs::Message, tr("Ошибка конфигурации датчика температуры.\n"
                              "Необходима повторная конфигурация.\n"
                              "Отключите и подключите датчик по новой.")}});
        return false;
    }

    // Уведомляем, что датчик сконфигурировался
    mStatus = SensorStatus::WaitReset;
    emit sensorEvent(SensorEvent::Configured);
    return true;
}

bool BMP280TemperatureSpiSensor::isConfig() const
{
    return mStatus == SensorStatus::WaitReset || mStatus == SensorStatus::WaitingStartDispatchSensorReadings || mStatus == SensorStatus::DispatchSensorReadings;
}

bool BMP280TemperatureSpiSensor::startSendingSensorReadings()
{
    if(!isInit() || !isSensorConnected()  ||
            !isConfig() || needMakeSensorReset())
    {
        return false;
    }

    // Если рассылка и так запущена
    if (isStartedSendingSensorReadings()) {
        return false;
    }

    // Устанавливаем состояние рассылки показаний и уведомляем
    mStatus = SensorStatus::DispatchSensorReadings;
    emit sensorEvent(SensorEvent::StartDispatchSensorReadings);

    return true;
}

bool BMP280TemperatureSpiSensor::isStartedSendingSensorReadings() const
{
    return mStatus == SensorStatus::DispatchSensorReadings;
}

bool BMP280TemperatureSpiSensor::isSensorConnected() const
{
    return mSensorConnected;
}

void BMP280TemperatureSpiSensor::endSendingSensorReadings()
{
    // Если рассылка показаний не идёт
    if (!isStartedSendingSensorReadings()) {
        return;
    }

    // Переводим в состояние ожидания запуска рассылки
    mStatus = SensorStatus::WaitingStartDispatchSensorReadings;
    emit sensorEvent(SensorEvent::StopDispatchSensorReadings);
}

bool BMP280TemperatureSpiSensor::needMakeSensorReset() const
{
    return mLastResetTimestamp <= 0;
}

bool BMP280TemperatureSpiSensor::canMakeSensorReset() const
{
    return isSensorConnected() && isConfig();
}

bool BMP280TemperatureSpiSensor::makeSensorReset()
{
    // Если не можем сделать сброс датчика
    if (!canMakeSensorReset()) {
        return false;
    }

    // Устанавливаем время сброса датчика
    mLastResetTimestamp = QDateTime::currentMSecsSinceEpoch();

    // Уведомляем об успешном сбросе
    mStatus = SensorStatus::WaitingStartDispatchSensorReadings;
    emit sensorEvent(SensorEvent::Reset);

    return true;
}

void BMP280TemperatureSpiSensor::softReset()
{
    mLastData.clear();
    mLastResetTimestamp = 0;
    memset(mTempResponse, 0, sizeof(mTempResponse));
    memset(mTempCompensationParam, 0, sizeof(mTempCompensationParam));
    mCountRequestError = 0;
}

void BMP280TemperatureSpiSensor::hardReset()
{

}

void BMP280TemperatureSpiSensor::checkSensorConnection()
{
    // Если не было инициализации
    if (!isInit()) {
        return;
    }

    // Для проверки текущего состояния датчика
    bool isConn {true};

    // Если датчик не подключён (на пине либо максимум либо минимум)
    if (((mTempResponse[0] == 0xFF) && (mTempResponse[1] == 0xFF) &&
            (mTempResponse[2] == 0xFF) && (mTempResponse[3] == 0xFF)) ||
            ((mTempResponse[0] == 0x0) && (mTempResponse[1] == 0x0) &&
            (mTempResponse[2] == 0x0) && (mTempResponse[3] == 0x0))) {
        isConn = false;
    }

    // Если датчик отключился во время снятия показаний
    if (mSensorConnected && mStatus == SensorStatus::DispatchSensorReadings && !isConn) {
        ++mCountRequestError;

        // Очищаем ошибочные данные
        mLastData.clear();

        // Если число ошибок соединения с датчиком достигло предела
        if (mCountRequestError == MaxCountRequestError) {
            endSendingSensorReadings();
            mSensorConnected = false;
            mStatus = SensorStatus::WaitConnection;
            emit sensorEvent(SensorEvent::ConnectionError, QVariantMap{
                                 {GlobalEventsArgs::Message, tr("Превышен лимит попыток получения ответа от датчика.\n"
                                  "Необходимо переподключить датчик.")}
            });
        }

        return;
    }

    // Если датчик просто подключился/отключился
    if (mSensorConnected != isConn) {
        // Мягкий сброс
        softReset();

        // Если датчик подключился
        if (isConn) {
            mSensorConnected = true;
            mStatus = SensorStatus::WaitConfigured;
            emit sensorEvent(SensorEvent::Connected);
        } else { // Если датчик отключился
            mSensorConnected = false;
            mStatus = SensorStatus::WaitConnection;
            emit sensorEvent(SensorEvent::Disconnected);
        }
    }

    // Если ошибки ранее были, то сбрасываем
    if (hasErrorCurrentRequest()) {
        mCountRequestError = 0;
    }
}

int64_t BMP280TemperatureSpiSensor::lastResetTimestamp() const
{
    return mLastResetTimestamp;
}

ComplexValue BMP280TemperatureSpiSensor::readData() const
{
    return mLastData;
}

SensorStatus BMP280TemperatureSpiSensor::status() const
{
    return mStatus;
}

QString BMP280TemperatureSpiSensor::info() const
{
    return tr("Датчик температуры");
}

bool BMP280TemperatureSpiSensor::isAllPropertiesToInitAreSet() const
{
    bool check = true;
    check &= !mPropertyMap[PathToSPIDeviceProperty].toString().isEmpty();
    check &= !mPropertyMap[SPISpeedProperty].toString().isEmpty();
    return check;
}

bool BMP280TemperatureSpiSensor::hasSensorProperty(const QString &key) const
{
    return mPropertyMap.contains(key);
}

QVariant BMP280TemperatureSpiSensor::sensorProperty(const QString &key) const
{
    if (key == PathToSPIDeviceProperty && hasSensorProperty(PathToSPIDeviceProperty)) {
        return mPropertyMap[PathToSPIDeviceProperty];
    }

    if (key == SPISpeedProperty && hasSensorProperty(SPISpeedProperty)) {
        return mPropertyMap[SPISpeedProperty];
    }

    return {};
}

QVariantMap BMP280TemperatureSpiSensor::sensorProperties() const
{
    return mPropertyMap;
}

bool BMP280TemperatureSpiSensor::setSensorProperty(const QString &key, const QVariant &value)
{
    bool isGood {false};

    // Проверка и установка свойства PathToSPIDeviceProperty
    if (key == PathToSPIDeviceProperty && hasSensorProperty(PathToSPIDeviceProperty) &&
            !propertyHasBeenSet(PathToSPIDeviceProperty))
    {
        isGood = isGoodFile(value.toString());
        if (isGood) {
            mPropertyMap[PathToSPIDeviceProperty] = value;
        }
    }

    // Проверка и установка свойства SPISpeedProperty
    if (key == SPISpeedProperty && hasSensorProperty(SPISpeedProperty ) &&
                !propertyHasBeenSet(SPISpeedProperty))
    {
        const int speed = value.toInt(&isGood);
        if (isGood && speed > 0 && speed <= 10000000) {
            mPropertyMap[SPISpeedProperty] = value;
        }
    }

    return isGood;
}

void BMP280TemperatureSpiSensor::setSensorProperties(const QVariantMap &properties)
{
    QMapIterator<QString, QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        setSensorProperty(it.key(), it.value());
    }
}

bool BMP280TemperatureSpiSensor::propertyHasBeenSet(const QString &key) const
{
    if (key == PathToSPIDeviceProperty && hasSensorProperty(PathToSPIDeviceProperty)) {
        return !mPropertyMap[PathToSPIDeviceProperty].toString().isEmpty();
    }

    if (key == SPISpeedProperty && hasSensorProperty(SPISpeedProperty)) {
        return !mPropertyMap[SPISpeedProperty].toString().isEmpty();
    }

    return false;
}

void BMP280TemperatureSpiSensor::resetPropertyMap()
{
    mPropertyMap[PathToSPIDeviceProperty].clear();
    mPropertyMap[SPISpeedProperty].clear();
}

void BMP280TemperatureSpiSensor::dataRequest()
{
    #ifdef QT_DEBUG
    if (avgBenchTime > 100) {
        qDebug() << "SPI interrogation, ms" << avgBenchTime / (float)benchCount;
        avgBenchTime = 0;
        benchCount = 0;
    }
    #endif

    // Обнуляем ответ на запрос и запрашиваем температуру
    memset(mTempResponse, 0, sizeof(mTempResponse));
    mSPIChannel->transferSPI(mTempRequest, mTempResponse, 4);

    // Преобразовываем в значение температуры и записываем время снятия показания
    mLastData.value = bmp280_compensate_T_int32((mTempResponse[1] << 12) | (mTempResponse[2] << 4) | (mTempResponse[3] >> 4)) / 100.0;
    mLastData.timestamp = QDateTime::currentMSecsSinceEpoch();
    mLastData.valid = true;

    // Проверка, подключён ли датчик
    checkSensorConnection();

    #ifdef QT_DEBUG
    avgBenchTime += (mLastData.timestamp - benchTime);
    benchTime = mLastData.timestamp;
    benchCount++;
    #endif

    // Если рассылка показаний включена
    if (isStartedSendingSensorReadings()) {
        emit dataReady();
    }
}

int32_t BMP280TemperatureSpiSensor::bmp280_compensate_T_int32(int32_t adc_T)
{
    return (((((((adc_T >> 3) - (mTempCompensationParam[0] << 1))) * (mTempCompensationParam[1])) >> 11)
            + ((((((adc_T >> 4) - (mTempCompensationParam[0])) * ((adc_T >> 4) - (mTempCompensationParam[0]))) >> 12)
            * (mTempCompensationParam[2])) >> 14)) * 5 + 128) >> 8;
}

bool BMP280TemperatureSpiSensor::hasErrorCurrentRequest()
{
    return mCountRequestError != 0;
}

bool BMP280TemperatureSpiSensor::configBMP280()
{
    // Если не было инициализации или датчик не подключён
    if (!isInit() || !isSensorConnected()) {
        return false;
    }

    // Конфигурируем работу датчика
    const uint8_t configTempSensor[] = {BMP280_REG_CTRL_MEAS & 0x7F, 0x63, BMP280_REG_CONFIG & 0x7F, 0x20};
    mSPIChannel->writeSPI(configTempSensor, 4);

    return true;
}

bool BMP280TemperatureSpiSensor::takeTemperatureCompensationParametersBMP280()
{
    // Если не было инициализации или датчик не подключён
    if (!isInit() || !isSensorConnected()) {
        return false;
    }

    // Получаем компенсирующие значения для вычисления температуры
    uint8_t compRegisters[7] {};
    const uint8_t getCompReg[6] {BMP280_REG_COMP_TEMP_1, BMP280_REG_COMP_TEMP_2,
                                 BMP280_REG_COMP_TEMP_3, BMP280_REG_COMP_TEMP_4,
                                 BMP280_REG_COMP_TEMP_5, BMP280_REG_COMP_TEMP_6};
    memset(mTempCompensationParam, 0, sizeof(mTempCompensationParam));
    mSPIChannel->transferSPI(getCompReg, compRegisters, 7);

    // Забираем компенсирующие значения
    mTempCompensationParam[0] = *(uint16_t *)(compRegisters + 1);
    mTempCompensationParam[1] = *(int16_t *)(compRegisters + 3);
    mTempCompensationParam[2] = *(int16_t *)(compRegisters + 5);

    // Если ответ датчика некорректный
    if ((mTempCompensationParam[0] == 65535 &&
        mTempCompensationParam[1] == -1 &&
        mTempCompensationParam[2] == -1) ||
        (mTempCompensationParam[0] == 0 &&
        mTempCompensationParam[1] == 0 &&
        mTempCompensationParam[2] == 0))
    {
        mTempCompensationParam[0] = 0;
        mTempCompensationParam[1] = 0;
        mTempCompensationParam[2] = 0;
        return false;
    }

    return true;
}
