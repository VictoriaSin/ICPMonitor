#include "fileimitsensor.h"
#include "global_functions.h"
#include <QDateTime>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

const QString FileImitSensor::FilePathProperty = "fileName";

FileImitSensor::FileImitSensor()
{
    // Инициализируем карту свойств
    mPropertyMap.insert(FilePathProperty, "");

    // Настраиваем таймер генерации данных
    mDataRequestTimer = new QTimer(this);
    mDataRequestTimer->setInterval(1000 / mReadPerSec);
    connect(mDataRequestTimer, &QTimer::timeout, this, &FileImitSensor::dataRequest);
}

bool FileImitSensor::init(const QVariantMap &args)
{
    // Если датчик уже инициализирован
    if (isInit()) {
        return false;
    }

    // Если не все свойства для инициализации установлены
    if (!isAllPropertiesToInitAreSet()) {
        setSensorProperties(args); // Установятся лишь ранее не установленные свойства
    }

    // Если не установлены все свойства для инициализации
    const bool isSetAllProperties = isAllPropertiesToInitAreSet();
    if (!isSetAllProperties) {
        emit sensorEvent(SensorEvent::InitError, QVariantMap{
                             {GlobalEventsArgs::Message, tr("Не установлены все необходимые свойства для инициализации")}});
        return false;
    }

    mStatus = SensorStatus::WaitConnection; // Устанавливаем ожидание подключения датчика
    mDataRequestTimer->start(); // Запускаем генерацию запросов данных датчика
    emit sensorEvent(SensorEvent::Inited);

    return true;
}

bool FileImitSensor::isInit() const
{
    return mStatus != SensorStatus::WaitInited;
}

bool FileImitSensor::config(const QString &/*jsonFilePath*/)
{
    // Если датчик уже сконфигурирован
    if (isConfig()) {
        return false;
    }

    // Если датчик не проинициализирован или не подключён (файл не считан)
    if (!isInit() || !isSensorConnected()) {
        return false;
    }

    // Уведомляем, что датчик сконфигурировался
    mStatus = SensorStatus::WaitReset;
    emit sensorEvent(SensorEvent::Configured);
    return true;
}

bool FileImitSensor::isConfig() const
{
    return mStatus == SensorStatus::WaitReset || mStatus == SensorStatus::WaitingStartDispatchSensorReadings || mStatus == SensorStatus::DispatchSensorReadings;
}

bool FileImitSensor::startSendingSensorReadings()
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

bool FileImitSensor::isStartedSendingSensorReadings() const
{
    return mStatus == SensorStatus::DispatchSensorReadings;
}

bool FileImitSensor::isSensorConnected() const
{
    return mFileIsRead;
}

void FileImitSensor::endSendingSensorReadings()
{
    // Если рассылка показаний не идёт
    if (!isStartedSendingSensorReadings()) {
        return;
    }

    // Переводим в состояние ожидания запуска рассылки
    mStatus = SensorStatus::WaitingStartDispatchSensorReadings;
    emit sensorEvent(SensorEvent::StopDispatchSensorReadings);
}

bool FileImitSensor::needMakeSensorReset() const
{
    return mLastResetTimestamp <= 0;
}

bool FileImitSensor::canMakeSensorReset() const
{
    return isSensorConnected() && isConfig();
}

bool FileImitSensor::makeSensorReset()
{
    // Если не можем сделать сброс датчика
    if (!canMakeSensorReset()) {
        return false;
    }

    // Устанавливаем время сброса датчика
    mLastResetTimestamp = QDateTime::currentMSecsSinceEpoch();

    // Уведомляем об успешном сбросе
    mStatus = SensorStatus::WaitingStartDispatchSensorReadings; // Ожидаем старта рассылки показаний
    emit sensorEvent(SensorEvent::Reset);

    return true;
}

void FileImitSensor::softReset()
{

}

void FileImitSensor::hardReset()
{
    endSendingSensorReadings(); // Останавливаем рассылку показаний
    mLastReading.clear(); // Очищаем последнее считанное показание
    mFileIsRead = false; // Указываем, что датчик не подключён
    mLastResetTimestamp = 0; // Обнуляем время последнего сброса датчика
    mSensorReadings.clear();  // Очищаем вычитанные показания из файла
    mTimeIter = 0; // Обнуляем итератор по считанным показаниям
    resetPropertyMap(); // Обнуляем все свойства
    mStatus = SensorStatus::WaitInited; // Переводим состояния в ожидание инициализации
    emit sensorEvent(SensorEvent::HardReset); // Уведомляем о полном сбросе датчика
}

void FileImitSensor::checkSensorConnection()
{
    // Если не было инициализации
    if (!isInit()) {
        return;
    }

    // Если файл считан
    if (isSensorConnected()) {
        return;
    }

    // Считываем файл
    mFileIsRead = readAllSensorReadingsFromFile();
    if (mFileIsRead) {
        mStatus = SensorStatus::WaitConfigured;
        emit sensorEvent(SensorEvent::Connected);
    }
}

int64_t FileImitSensor::lastResetTimestamp() const
{
    return mLastResetTimestamp;
}

ComplexValue FileImitSensor::readData() const
{
    return mLastReading;
}

SensorStatus FileImitSensor::status() const
{
    return mStatus;
}

QString FileImitSensor::info() const
{
    const QString filePathProp = sensorProperty(FilePathProperty).toString();
    if (filePathProp.isEmpty()) {
        return tr("Не указан файл имитации данных");
    }
    return QString("%1: %2").arg(tr("Имитация датчика ВЧД")).arg(QFileInfo(filePathProp).fileName());
}

bool FileImitSensor::hasSensorProperty(const QString &key) const
{
    return mPropertyMap.contains(key);
}

QVariant FileImitSensor::sensorProperty(const QString &key) const
{
    if (key == FilePathProperty && hasSensorProperty(FilePathProperty)){
        return mPropertyMap[FilePathProperty];
    }
    return {};
}

QVariantMap FileImitSensor::sensorProperties() const
{
    return mPropertyMap;
}

bool FileImitSensor::setSensorProperty(const QString &key, const QVariant &value)
{
    bool isGood {false};

    if (key == FilePathProperty && hasSensorProperty(FilePathProperty) &&
            !propertyHasBeenSet(FilePathProperty))
    {
        isGood = isGoodFile(value.toString());
        if (isGood) {
            mPropertyMap[FilePathProperty] = value;
        }
    }

    return isGood;
}

void FileImitSensor::setSensorProperties(const QVariantMap &properties)
{
    QMapIterator<QString, QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        setSensorProperty(it.key(), it.value());
    }
}

bool FileImitSensor::propertyHasBeenSet(const QString &key) const
{
    if (key == FilePathProperty && hasSensorProperty(FilePathProperty)) {
        return !mPropertyMap[FilePathProperty].toString().isEmpty();
    }
    return false;
}

bool FileImitSensor::isAllPropertiesToInitAreSet() const
{
    bool check = true;
    check &= !mPropertyMap[FilePathProperty].toString().isEmpty();
    return check;
}

void FileImitSensor::resetPropertyMap()
{
    mPropertyMap[FilePathProperty].clear();
}

void FileImitSensor::dataRequest()
{
    // Очищаем последнее показание
    mLastReading.clear();

    // Проверка, считан ли файл
    checkSensorConnection();

    // Берём следующие данные из файла
    mLastReading.value = mSensorReadings[mTimeIter % mSensorReadings.size()];
    mLastReading.valid = true;
    mLastReading.timestamp = QDateTime::currentMSecsSinceEpoch();

    // Если идёт рассылка данных
    if (isStartedSendingSensorReadings()) {
        emit dataReady();
    }

    mTimeIter++;
}

bool FileImitSensor::readAllSensorReadingsFromFile()
{
    // Если не датчик не инициализирован
    if (!isInit()) {
        return false;
    }

    // Если файл с показаниями прочитан
    if (isSensorConnected()) {
        return false;
    }

    // Открываем файл для чтения
    QFile file(sensorProperty(FilePathProperty).toString());
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Вычитываем файл
    QTextStream sdata(&file);
    bool okPars = false;
    while (!sdata.atEnd())
    {
        const QString line = sdata.readLine();
        const float value = line.toFloat(&okPars);
        if (okPars) {
            mSensorReadings.append(value);
        }
    }

    // Закрываем файл
    file.close();

    // Если данных в файле не было
    if(mSensorReadings.size() == 0){
        return false;
    }

    return true;
}
