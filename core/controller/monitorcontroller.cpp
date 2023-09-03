#include "monitorcontroller.h"
#include "controller/settings.h"
#include "controller/averageicpcontroller.h"
#include "controller/alarmcontroller.h"
#include "controller/sessions/sessionmanager.h"
#include "controller/sessions/session.h"
#include "controller/labels/labelmanager.h"
#include "controller/labels/label.h"
#include "controller/databasemanager.h"
#include "controller/fileProcessing/filecontroller.h"
#include "controller/fileProcessing/cyclefilewriter.h"
#include "blockDevices/blockdevicemanager.h"
#include "blockDevices/blockdevice.h"
#include "controller/screenwriter.h"
#include "sensor/impl/fileimitsensor.h"
#include "sensor/impl/bmp280temperaturespisensor.h"
#include "global_functions.h"

#include <QDebug>
#include <QProcess>
#include <QDir>

LabelManager *mLabelManagerGlobal;
bool isLabelCreating {false};

//BlockDevice mMainBlockDevice

MonitorController::MonitorController() //: mWorkerThread(new QThread(this))
{
    // Создание настроек приложения и их чтение
    #ifdef PC_BUILD
        mICPSettings = new Settings("ICPMonitorSettings.ini");
    #else
        mICPSettings = new Settings("/opt/ICPMonitor/bin/ICPMonitorSettings.ini");
    #endif

    // Создание контроллера средних значений датчиков
    mAverageICPController = new AverageICPController(mICPSettings, this);

    // Создание контроллера тревоги датчиков
    mAlarmController = new AlarmController(mICPSettings, mAverageICPController, this);

    // Создание контроллера блочных устройств
    mBlockDeviceManager = new BlockDeviceManager;

    // Создание менеджера базы данных приложения
    mDataBaseManager = new DataBaseManager;

    // Создание менеджера сессий и меток
    mSessionManager = new SessionManager(mDataBaseManager);
    mLabelManagerGlobal = new LabelManager(mICPSettings, mDataBaseManager);

    // Создание файлового контроллера
    mFileController = new FileController(mICPSettings, mLabelManagerGlobal, this);

    // Усреднённые данные с датчика готовы
    connect(mAverageICPController,  &AverageICPController::dataReady,
                             this,  &MonitorController::processAverageSensorValue);
//    // Остановка mAverageTimer
//    connect(this,                   &MonitorController::stopAverageTimer,
//            mAverageICPController,  &AverageICPController::stop);
    // Прослушка событий тревоги
    connect(mAlarmController,       &AlarmController::alarmEvent,
                             this,  &MonitorController::processAlarmEvent);
    // Прослушка событий контроллера блочных устройств
    connect(mBlockDeviceManager  ,  &BlockDeviceManager::blockDevicesControllerEvent,
                             this,  &MonitorController::processBlockDevicesControllerEvent);
    // Прослушка событий файлового контроллера
    connect(mFileController      ,  &FileController::fileControllerEvent,
                             this,  &MonitorController::processFileControllerEvent);
    // События контроллера для записи файлов
    connect(               this,    &MonitorController::controllerEvent,
                mFileController,    &FileController::controllerEventHandler);

    // Скидывание в поток и его запуск
    mWorkerThread = new QThread(this);
    mBlockDeviceManager->moveToThread(mWorkerThread);
    mWorkerThread->start();
}
void MonitorController::init()
{
    qDebug() << "Init System";
    // Инициализация системы
    runState(State::InitSystem);
}
void MonitorController::deinit()
{
    qDebug() << "Deinit System";
    // Деинициализация системы
    runState(State::DeinitSystem);
}

void MonitorController::updateSoftwareStorage()
{
    qDebug() << "Update Software Storage";
    if (!isInitSoftwareStorage()) {
        runState(State::DeinitSoftwareStorage);
        return;
    }

    // Обновляем
    mSoftwareStorage->refresh();

    // Обновляем БД
    runState(State::UpdateDataBase);

    // Обновляем контроллер файлов
    runState(State::UpdateFileController);
}
void MonitorController::deinitSoftwareStorage()
{
    qDebug() << "Deinit Software Storage";

    // Денициализируем базу данных
    runState(State::DeinitDataBase);

    // Размонтируем
    if (mSoftwareStorage) {
        QTimer::singleShot(0, mBlockDeviceManager, [this, blockDev = mSoftwareStorage](){
            mBlockDeviceManager->umountBlockDevice(blockDev);
        });
    }

    // Освобождаемся
    mSoftwareStorage.reset();
    softwareStorageUnavailable();

    // Обновляем контроллер файлов
    runState(State::UpdateFileController);
}
void MonitorController::initDataBase()
{
    qDebug() << "Init DataBase";
    if (!mDataBaseManager) {
        runState(State::DeinitDataBase);
        return;
    }

    // Открываем БД
    bool isOpenDB = mDataBaseManager->isOpenConnection();
    if (!isOpenDB) {
        isOpenDB = mDataBaseManager->openDataBase(mSoftwareStorage->getLastMountPoint());
    }

    // Открылась ли БД
    if (isOpenDB) {
        dataBaseAvailable();
    } else {
        qDebug() << "Trouble Init DB";
        ++mCurrentNumOpenDataBase;
        if (mCurrentNumOpenDataBase < MaxTryOpenDataBase) {
            QTimer::singleShot(mIntervalOpenDataBaseMs, this, [this](){runState(State::InitDataBase);});
        } else {
            runState(State::DeinitDataBase);
        }
    }
}
void MonitorController::deinitDataBase()
{
    qDebug() << "Deinit DataBase 2";
    if (mDataBaseManager) {
        mDataBaseManager->closeDataBase();
    }
    dataBaseUnavailable();
}
void MonitorController::updateDataBase()
{
    qDebug() << "Update DataBase";
    if (!mDataBaseManager || !isInitSoftwareStorage()) {
        runState(State::DeinitDataBase);
        return;
    }

    // Информация о файле БД
    const QFileInfo fileDBInfo = mDataBaseManager->dataBaseFile();
    if (!fileDBInfo.exists()) {
        runState(State::DeinitDataBase);
        return;
    }

    // Если не совпадают пути БД и блочного устройства
    const QString mntPoint = mSoftwareStorage->getLastMountPoint();
    if (!mntPoint.isEmpty() && !fileDBInfo.absolutePath().contains(mntPoint)) {
        runState(State::ReinitDataBase);
    }
}
void MonitorController::dataBaseAvailable()
{
    qDebug() << "DataBase Available";
    mDataBaseState = ControllerEvent::DataBaseAvailable;
    emit controllerEvent(mDataBaseState);
    mCurrentNumOpenDataBase = 0;
}
void MonitorController::dataBaseUnavailable()
{
    qDebug() << "DataBase Unavailable 1";
    mDataBaseState = ControllerEvent::DataBaseUnavailable;
    emit controllerEvent(mDataBaseState);
    mCurrentNumOpenDataBase = 0;
}
void MonitorController::updateFileController()
{
    qDebug() << "Update FileController";
    mFileController->setSoftwareStorage(mSoftwareStorage);
}
void MonitorController::initRTCModule()
{

}
bool MonitorController::isInitSoftwareStorage()
{
    // Если программное хранилище отсутствует
    if (!mSoftwareStorage) {
        return false;
    }

    mSoftwareStorage->refresh();

    // Если программное хранилище не валидное (извлечено)
    if (!mSoftwareStorage->isValid()) {
        return false;
    }

    return mSoftwareStorage->isMounted();
}
ControllerEvent MonitorController::getSoftwareStorageState() const
{
    return mSoftwareStorageState;
}
void MonitorController::setSoftwareStorage(const std::shared_ptr<BlockDevice> &devStorage)
{
    if (!devStorage || !devStorage->isValid()) {
        emit controllerEvent(ControllerEvent::NewSoftwareStorageError, QVariantMap{{GlobalEventsArgs::Message,
                                                                                    tr("Программное хранилище\n"
                                                                                    "не указано или не валидно!")}});
        return;
    }

    // Получаем UUID
    const QString devUUID = devStorage->getUUID();
    if (devUUID.isEmpty()) {
        emit controllerEvent(ControllerEvent::NewSoftwareStorageError, QVariantMap{{GlobalEventsArgs::Message,
                                                                                    tr("У программного хранилища\n"
                                                                                    "отсутствует UUID!")}});
        return;
    }

    // Если есть активная сессия
    if (mSessionManager->isActiveSession()) {
        emit controllerEvent(ControllerEvent::NewSoftwareStorageError, QVariantMap{{GlobalEventsArgs::Message,
                                                                                    tr("Программное хранилище\n"
                                                                                    "не было сменено.\n"
                                                                                    "Сессия активна!")}});
        return;
    }

    // Переводим в состояние недоступности программного хранилища
    runState(State::DeinitSoftwareStorage);

    // Сохраняем новое программное хранилище
    qDebug() << "New Software Storage";
    mICPSettings->setSoftwareStorageUUID(devUUID);
    mICPSettings->writeGeneralSettings();

    // Инициализация нового программного хранилища
    runState(State::InitSoftwareStorage);
}
std::shared_ptr<BlockDevice> MonitorController::getSoftwareStorage() const
{
    return mSoftwareStorage;
}
QList<std::shared_ptr<BlockDevice> > MonitorController::getAvailableBlockDevices() const
{
    if (!mBlockDeviceManager) {
        return {};
    }
    return mBlockDeviceManager->getAvailableBlockDevices();
}
void MonitorController::initSensor()
{
    // Создание определённого датчика
    QVariantMap initMap;
    mSensorThread = new QThread(this);

#ifdef TEST_BUILD
    mSensor = new FileImitSensor;
    initMap.insert(FileImitSensor::FilePathProperty, ":/imit_data/ICP_data_60sec.csv");
#else
    #ifdef PC_BUILD
        mSensor = new FileImitSensor;
        initMap.insert(FileImitSensor::FilePathProperty, ":/imit_data/ICP_data_60sec.csv");
    #else
        mSensor = new BMP280TemperatureSpiSensor;
        initMap.insert(BMP280TemperatureSpiSensor::PathToSPIDeviceProperty, "/dev/spidev0.0");
        initMap.insert(BMP280TemperatureSpiSensor::SPISpeedProperty, 100000);
    #endif
#endif
    mSensor->moveToThread(mSensorThread);
    mSensorThread->start(QThread::HighPriority);

    connect(mSensor, &ISensor::dataReady, this, &MonitorController::processSensorData);
    connect(mSensor, &ISensor::sensorEvent, this, &MonitorController::processSensorEvent);

    // Запускаем инициализацию датчика (АЦП)
    QTimer::singleShot(0, mSensor, [this, initMap](){
        mSensor->init(initMap);
    });
}
void MonitorController::terminate()
{
    // Останавливаем рассылку показаний датчика
    QTimer::singleShot(0, mSensor, [this](){
        mSensor->endSendingSensorReadings();
    });

    // Прекращаем работу контроллеров
    mAverageICPController->terminate();
    mAlarmController->terminate();
    mFileController->terminate();

    // Прекращаем работу менеджеров
    mSessionManager->terminate();
    mLabelManagerGlobal->terminate();
    mDataBaseManager->terminate();
    QTimer::singleShot(0, mBlockDeviceManager, [this](){
        mBlockDeviceManager->terminate();
    });

    // Прекращаем запущенные потоки

    mSensorThread->quit();
    mSensorThread->wait(1000);
    mWorkerThread->quit();
    mWorkerThread->wait(5000);

    // Сохраняем настройки
    //mICPSettings->writeAllSetting();
}
Settings *MonitorController::settings() const
{
    return mICPSettings;
}
const ComplexValue &MonitorController::getLastConvertedSensorValue() const
{
    return mLastConvertedSensorValue;
}
const ComplexValue MonitorController::getLastSensorValue() const
{
    if (!mSensor) {
        return {};
    }

    return mSensor->readData();
}
ComplexValue MonitorController::getLastAverageValue() const
{
    return mAverageICPController->lastAverageValue();
}
bool MonitorController::setAppLanguage(QLocale::Language language)
{
    // Установка языка приложения
    bool isOk = mICPSettings->setAppLanguage(language);

    // Сохраняем все настройки
    if (isOk) {
        mICPSettings->writeAllSetting();
    }

    return isOk;
}
void MonitorController::makeLabel()
{
    // Если текущая сессия отсутствует или локальная
    const auto &currentSession = mSessionManager->getCurrentSession();
    if (!currentSession || currentSession->getId() == LOCAL_SESSION_ID) {
        return;
    }
    isLabelCreating = true;
    // Если метка создалась, то рассылаем уведомление о создании с номером метки и временем создания
    if (std::shared_ptr<Label> label = mLabelManagerGlobal->createLabel())
    {
        emit controllerEvent(ControllerEvent::LabelCreated, QVariantMap({
                                           {ControllerEventsArgs::LabelNumber,           label->getNumberLabel()},
                                           {ControllerEventsArgs::LabelCreationTimeMs,   (qlonglong)label->getTimeStartLabelMS()}  }));
    }
}

void MonitorController::deleteLabel()//доделать
{
    mLabelManagerGlobal->deleteLabel();
    //isLabelCreating = false;
}

int MonitorController::getLabelsCountPerCurrentSession() const
{
    if (!mLabelManagerGlobal) {
        return 0;
    }

    return mLabelManagerGlobal->getLabelsCountPerCurrentSession();
}
std::shared_ptr<Session> MonitorController::getCurrentSession() const
{
    if (!mSessionManager) {
        return {};
    }

    return mSessionManager->getCurrentSession();
}
QVector<std::shared_ptr<Label>> MonitorController::getAllLabelsBelongSession(int sessionID)
{

    return mLabelManagerGlobal->getAllLabelsBelongSession(sessionID);
}
LabelManager *MonitorController::getLabelManager() const
{
    return mLabelManagerGlobal;
}
bool MonitorController::testMakeSensorReset()
{
    if(!mSensor){
        return false;
    }

    QTimer::singleShot(0, mSensor, [this](){
        mSensor->makeSensorReset();
    });

    return true;
}
void MonitorController::runState(State state)
{
    switch (state)
    {
    case State::InitSystem: {
        initSoftwareStorage();
        initRTCModule();
        initSensor();
        break;
    }
    case State::DeinitSystem: {
        // Пока не понятно, поэтому используем terminate
        terminate();
        break;
    }
    case State::ReinitSystem: {
        break;
    }
    case State::InitSoftwareStorage: {
        initSoftwareStorage();
        break;
    }
    case State::DeinitSoftwareStorage: {
        deinitSoftwareStorage();
        break;
    }
    case State::UpdateSoftwareStorage: {
        updateSoftwareStorage();
        break;
    }
    case State::InitDataBase: {
        initDataBase();
        break;
    }
    case State::DeinitDataBase: {
        deinitDataBase();
        break;
    }
    case State::ReinitDataBase: {
        deinitDataBase();
        initDataBase();
        break;
    }
    case State::UpdateDataBase: {
        updateDataBase();
        break;
    }
    case State::UpdateFileController: {
        updateFileController();
        break;
    }
    case State::InitRTCModule: {
        initRTCModule();
        break;
    }
    case State::InitSensor: {
        initSensor();
        break;
    }
    default: break;
    }
}
//void MonitorController::stopAverageTimer()
//{
//    mAverageICPController->mAverageTimer->stop();
//}
void MonitorController::softwareStorageConnected()
{
    qDebug() << "ProgramStorageConnected";
    runState(State::InitSoftwareStorage);
}
void MonitorController::softwareStorageDisconnected()
{
    qDebug() << "ProgramStorageDisconnected";
    runState(State::DeinitSoftwareStorage);
}
void MonitorController::softwareStorageChanged()
{
    qDebug() << "ProgramStorageChanged";
    runState(State::UpdateSoftwareStorage);
}
void MonitorController::softwareStorageAvailable()
{
    qDebug() << "Software Storage Available";
    mSoftwareStorageState = ControllerEvent::SoftwareStorageAvailable;
    emit controllerEvent(mSoftwareStorageState);
    mCurrentNumMountSoftwareStorage = 0;
}
void MonitorController::softwareStorageUnavailable()
{
    qDebug() << "Software Storage Unavailable";
    mSoftwareStorageState = ControllerEvent::SoftwareStorageUnavailable;
    emit controllerEvent(mSoftwareStorageState);
    mCurrentNumMountSoftwareStorage = 0;
}
void MonitorController::softwareStorageNotAssigned()
{
    qDebug() << "Software Storage Not Assigned";
    mSoftwareStorageState = ControllerEvent::SoftwareStorageNotAssigned;
    emit controllerEvent(mSoftwareStorageState);
    mCurrentNumMountSoftwareStorage = 0;
}
const ISensor *MonitorController::sensor() const
{
    return mSensor;
}
bool MonitorController::setLevelsAndStatesAlarm(int lowLevelAlarm, int highLevelAlarm, bool lowEnabel, bool highEnable)
{
    if (!mICPSettings || lowLevelAlarm >= highLevelAlarm) {
        return false;
    }

    const int LLA = mICPSettings->getLowLevelAlarm();
    const int HLA = mICPSettings->getHighLevelAlarm();
    const bool LLSA = mICPSettings->getLowLevelStateAlarm();
    const bool HLSA = mICPSettings->getHighLevelStateAlarm();

    // Если одно из значений уровня тревоги изменилось
    if (LLA != lowLevelAlarm || HLA != highLevelAlarm){
        mICPSettings->setLowLevelAlarm(lowLevelAlarm);
        mICPSettings->setHighLevelAlarm(highLevelAlarm);
        emit controllerEvent(ControllerEvent::UpdatedAlarmLevels);
    }

    // Если одно из состояний изменилось
    if (LLSA != lowEnabel || HLSA != highEnable) {
        mICPSettings->setLowLevelStateAlarm(lowEnabel);
        mICPSettings->setHighLevelStateAlarm(highEnable);
        emit controllerEvent(ControllerEvent::UpdatedAlarmStates);
    }

    // Сохраняем настройки
    mICPSettings->writeAlarmSettings();

    return true;
}
bool MonitorController::setInetrvalsOnGraph(double mCurrentReadingsGraphIntervalX, double mCurrentReadingsGraphIntervalYFrom, double mCurrentReadingsGraphIntervalYTo,
                                            double mTickCountX, double mTickCountY)
{
    if (!mICPSettings || mCurrentReadingsGraphIntervalYFrom >= mCurrentReadingsGraphIntervalYTo) {
        return false;
    }

    const double CXR = mICPSettings->getCurrentReadingsGraphIntervalX();
    const double CYRL = mICPSettings->getCurrentReadingsGraphIntervalY().first;
    const double CYRH = mICPSettings->getCurrentReadingsGraphIntervalY().second;
    const float TCX = mICPSettings->getCurrentTickCountX();
    const float TCY = mICPSettings->getCurrentTickCountY();


    // Если одно из значений осей изменилось
    if (CXR  != mCurrentReadingsGraphIntervalX ||
        CYRL != mCurrentReadingsGraphIntervalYFrom ||
        CYRH != mCurrentReadingsGraphIntervalYTo)
    {
        mICPSettings->setCurrentReadingsGraphIntervalX(mCurrentReadingsGraphIntervalX);
        mICPSettings->setCurrentReadingsGraphIntervalY(mCurrentReadingsGraphIntervalYFrom, mCurrentReadingsGraphIntervalYTo);
        emit controllerEvent(ControllerEvent::UpdateGraphIntervals);
    }

    if (TCX != mTickCountX || TCY != mTickCountY)
    {
        mICPSettings->setCurrentTickCountX(mTickCountX);
        mICPSettings->setCurrentTickCountY(mTickCountY);
        emit controllerEvent(ControllerEvent::UpdateGraphTicks);
    }

    // Сохраняем настройки
    mICPSettings->writeCurrentSensorReadingsSettings();

    return true;
}
bool MonitorController::setGeneralParam(float mFontScaleFactor)
{
    if (!mICPSettings) {
        return false;
    }

    const float FSF = mICPSettings->getFontScaleFactor();
    const QString SSUUID= mICPSettings->getSoftwareStorageUUID();

    // Если одно из значений осей изменилось
    if (FSF  != mFontScaleFactor)
    {
        mICPSettings->setFontScaleFactor(mFontScaleFactor);
        emit controllerEvent(ControllerEvent::UpdateGeneralGroupInfo);
    }

    // Сохраняем настройки
    mICPSettings->writeGeneralSettings();

    return true;
}
void MonitorController::saveCurrentDateTime()
{
    mICPSettings->setLastSavedDateTimestampSec(QDateTime::currentDateTime().toSecsSinceEpoch());
    mICPSettings->writeGeneralSettings();
}
void MonitorController::writeScreenFromLinuxFB()
{
    mFileController->writeScreenFromLinuxFB();
}
void MonitorController::writeScreenFromWidget(const QPixmap &screen)
{
    mFileController->writeScreenFromWidget(screen);
}
void MonitorController::setMaxScreens(uint maxScreens)
{
    if (!mICPSettings) {
        return;
    }

    const uint CurrentMaxScreens = mICPSettings->getMaxScreens();
    if (CurrentMaxScreens == maxScreens) {
        return;
    }

    mICPSettings->setMaxScreens(maxScreens);
    mICPSettings->writeScreensSettings();

    emit controllerEvent(ControllerEvent::UpdatedMaxScreens);
}
void MonitorController::exportLastAbsoluteTimeCurrentSensorReadings(uint64_t exportTimeMs, uint32_t exportValues, ExportDataFunc func)
{
//    // Проверяем, есть ли данные для экспорта
//    if (getCountCurrentSensorReading() < 1) {
//        emit controllerEvent(ControllerEvent::ExportError, QVariantMap({
//             {keyfilecontrollerevent::errorMessage, tr("Текущие показания датчика отсутствуют.")}}));
//        return;
//    }

//    // Создаем базовую папку для хранения текущих показаний датчика, если её не существует на флешке
//    const QString exportPath = Settings::BasePathForMount + '/' + mICPSettings->getRelativeCurrentSensorReadingsPath();
//    if (!makeDir(exportPath)) {
//        emit controllerEvent(ControllerEvent::ExportError, QVariantMap({
//             {keyfilecontrollerevent::errorMessage, tr("Не удалось создать папку на\nфлеш-накопителе.")}}));
//        return;
//    }

//    mFileController->exportLastAbsoluteTimeCurrentSensorReadingsMs(exportPath, exportTimeMs, exportValues, func);
}
void MonitorController::exportLastAbsoluteTimeAverageSensorReadings(uint64_t exportTimeSec, uint32_t exportValues, ExportDataFunc func)
{
//    // Проверяем, есть ли данные для экспорта
//    if (getCountAverageSensorReading() < 1) {
//        emit controllerEvent(ControllerEvent::ExportError, QVariantMap({
//             {keyfilecontrollerevent::errorMessage, tr("Средние показания датчика отсутствуют.")}}));
//        return;
//    }

//    // Создаем базовую папку для хранения средних показаний датчика, если её не существует на флешке
//    const QString exportPath = Settings::BasePathForMount + '/' + mICPSettings->getRelativeAverageSensorReadingsPath();
//    if (!makeDir(exportPath)) {
//        emit controllerEvent(ControllerEvent::ExportError, QVariantMap({
//             {keyfilecontrollerevent::errorMessage, tr("Не удалось создать папку на\nфлеш-накопителе.")}}));
//        return;
//    }
//    mFileController->exportLastAbsoluteTimeAverageSensorReadingsSec(exportPath, exportTimeSec, exportValues, func);
}
QVector<ComplexValue> MonitorController::getAverageSensorReadingsFromTheInterval(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds)
{
    // Если начальная точка интервала больше конечной
    if (intervalStartTimeSeconds > intervalEndTimeSeconds) {
        return {};
    }

    // Если контроллер файлов отсутствует
    if (!mFileController) {
        return {};
    }

    // Возвращаем отобранные значения
    return mFileController->getAverageSensorReadingsFromTheInterval(intervalStartTimeSeconds, intervalEndTimeSeconds);
}
QVector<ComplexValue> MonitorController::getAverageSensorReadingsFromTheIntervalForTheCurrentSession(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds)
{
    if (!mFileController || !mSessionManager) {
        return {};
    }

    const auto &session = mSessionManager->getCurrentSession();
    if (!session) {
        return {};
    }

    // Если начальная точка интервала больше конечной
    if (intervalStartTimeSeconds > intervalEndTimeSeconds) {
        return {};
    }

    // Возвращаем отобранные значения
    return mFileController->getAverageSensorReadingsFromTheIntervalForTheSession(intervalStartTimeSeconds, intervalEndTimeSeconds, session->getId());
}
void MonitorController::setDateTime(int64_t timestamp)
{
    // Если сессия существует, то запрещаем сбрасывать время
    if (mSessionManager->getCurrentSession()) {
        return;
    }

    // Меняем системное время
    QProcess process;
    process.start("date", QStringList() << "+%s" << "-s" << QString("@%1").arg(timestamp));
    process.waitForFinished();
    process.close();
    emit controllerEvent(ControllerEvent::GlobalTimeUpdate);

    // Сохраняем текущее время системы в файл настроек
    saveCurrentDateTime();
}
bool MonitorController::dateTimeIsValid(const QDateTime &checkDate) const
{
    if (checkDate <= Settings::MinDateTime ||
            checkDate <= QDateTime::fromSecsSinceEpoch(mICPSettings->getLastSavedDateTimestampSec())) {
        return false;
    }
    return true;
}
bool MonitorController::currentTimeIsValid() const
{
    return dateTimeIsValid(QDateTime::currentDateTime());
}
void MonitorController::exportScreens(ConversionPictureFormat format, int countTransScreens)
{
    mFileController->exportScreenshots(format, countTransScreens);
}
void MonitorController::createSession()
{
    auto isCreated = mSessionManager->createSession();
    if (isCreated) {
        auto idSession = isCreated->getId();
        mLabelManagerGlobal->setSessionID(idSession);
        mFileController->setSessionID(idSession);
        emit controllerEvent(ControllerEvent::SessionStarted);
    }
}
void MonitorController::closeSession()
{
    bool isClosed = mSessionManager->closeCurrentSession();
    if (isClosed) {
        mLabelManagerGlobal->setSessionID(-1);
        mFileController->setSessionID(-1);
        emit controllerEvent(ControllerEvent::SessionClosed);
    }
}
void MonitorController::processSensorData()
{
    if (!mSensor)
    {
        return;
    }

    // Берём у датчика последнее показание
    mLastSensorValue = getLastSensorValue();

    // Файлы писать строго в тех единицах, которые выдаются датчиком
    mFileController->writeICPSensorData(&mLastSensorValue);

    // Средние значения строго в тех единицах, которые выдаются датчиком
    mAverageICPController->setSensorData(mLastSensorValue);

    // Конвертируем показание в указанный пользователем тип
    mLastConvertedSensorValue = mLastSensorValue;

    emit dataReadyForGraph();
}
void MonitorController::processSensorEvent(SensorEvent event, const QVariantMap &args)
{
    switch (event) {
    case SensorEvent::Inited:
        qDebug() << "Inited Sensor";
        emit controllerEvent(ControllerEvent::SensorInited, args);
        break;
    case SensorEvent::InitError:
        qDebug() << "InitError Sensor";
        closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorInitError, args);
        break;
    case SensorEvent::Connected:
        qDebug() << "Connected Sensor";
        QTimer::singleShot(0, mSensor, [this](){
            mSensor->config("");
        });
        emit controllerEvent(ControllerEvent::SensorConnected, args);
        break;
    case SensorEvent::ConnectionError:
        qDebug() << "ConnectionError Sensor";
        closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorConnectionError, args);
        break;
    case SensorEvent::Configured:
        qDebug() << "Configured Sensor";
        emit controllerEvent(ControllerEvent::SensorConfigured, args);
        break;
    case SensorEvent::ConfigError:
        qDebug() << "ConfigError Sensor";
        closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorConfigError, args);
        break;
    case SensorEvent::Reset:
        qDebug() << "Reset Sensor";
        /*!!!QTimer::singleShot(0, mSensor, [this](){
            mSensor->startSendingSensorReadings(); // Запускаем рассылку показаний
        });*/
        emit controllerEvent(ControllerEvent::SensorReset, args);
        break;
    case SensorEvent::ResetError:
        qDebug() << "ResetError Sensor";
        // Необходимо уведомить пользователя args об ошибке
        closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorResetError, args);
        break;
    case SensorEvent::StartDispatchSensorReadings:
        qDebug() << "Start Dispatch Sensor Readings";
        createSession();
        mAverageICPController->start();
        emit controllerEvent(ControllerEvent::SensorStartDispatchSensorReadings, args);
        break;
    case SensorEvent::StopDispatchSensorReadings:
        qDebug() << "Stop Dispatch Sensor Readings";
        //closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorStopDispatchSensorReadings, args);
        break;
    case SensorEvent::Disconnected:
        qDebug() << "Disconnected Sensor";
        closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorDisconnected, args);
        break;
    case SensorEvent::HardReset:
        qDebug() << "HardReset Sensor";
        closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorHardReseted, args);
        break;
    case SensorEvent::UndefinedError:
        qDebug() << "UndefinedError Sensor";
        // Необходимо уведомить пользователя args об ошибке
        closeSession();
        mAverageICPController->stop();
        mAlarmController->clear();
        emit controllerEvent(ControllerEvent::SensorUndefinedError, args);
        break;
    }
}
void MonitorController::processAlarmEvent(AlarmEvent event, const QVariantMap &args)
{
    switch (event) {
    case AlarmEvent::Low: {
        emit controllerEvent(ControllerEvent::AlarmLowOn, args);
        break;
    }
    case AlarmEvent::Normal: {
        emit controllerEvent(ControllerEvent::AlarmOff, args);
        break;
    }
    case AlarmEvent::High: {
        emit controllerEvent(ControllerEvent::AlarmHighOn, args);
        break;
    }
    default: break;
    }
}
void MonitorController::processFileControllerEvent(FileControllerEvent event, const QVariantMap &args)
{
    switch (event) {
    case FileControllerEvent::ExportProgress: {
        emit controllerEvent(ControllerEvent::ExportProgress, args);
        break;
    }
    case FileControllerEvent::ExportError: {
        emit controllerEvent(ControllerEvent::ExportError, args);
        break;
    }
    case FileControllerEvent::ExportDone: {
        emit controllerEvent(ControllerEvent::ExportDone, args);
        break;
    }
    case FileControllerEvent::ScreenWritten: {
        emit controllerEvent(ControllerEvent::ScreenWritten, args);
        break;
    }
    case FileControllerEvent::ScreenNotWritten: {
        emit controllerEvent(ControllerEvent::ScreenNotWritten, args);
        break;
    }
    case FileControllerEvent::FatalWriteCurrentSensorReadingsError: {
        closeSession();
        emit controllerEvent(ControllerEvent::RecordCurrentSensorReadingsInterruptedError, QVariantMap{{GlobalEventsArgs::Message, tr("Переполнена очередь записи\n"
                                                                                                        "текущих показаний в память!\n"
                                                                                                        "Сессия завершена!")}});
        break;
    }
    case FileControllerEvent::FatalWriteAverageSensorReadingsError: {
        closeSession();
        emit controllerEvent(ControllerEvent::RecordAverageSensorReadingsInterruptedError, QVariantMap{{GlobalEventsArgs::Message, tr("Переполнена очередь записи\n"
                                                                                                        "средних показаний в память!\n"
                                                                                                        "Сессия завершена!")}});
        break;
    }
    }
}
void MonitorController::processBlockDevicesControllerEvent(BlockDeviceManagerEvent event, const QVariantMap &args)
{
    switch (event) {
    case BlockDeviceManagerEvent::BlockDeviceConnected: {
        const QString devUUID = args.value(BlockDeviceManagerEventsArgs::UUID).toString();
        const QString devName = args.value(BlockDeviceManagerEventsArgs::DEV).toString();
        if (devName.isEmpty() || devUUID.isEmpty()) {
            return;
        }

        emit controllerEvent(ControllerEvent::BlockDeviceConnected, args);
        qDebug() << "BlockDeviceConnected";

        // Если подключено программное хранилище
        if (devUUID == mICPSettings->getSoftwareStorageUUID()) {
            softwareStorageConnected();
        }
        break;
    }
    case BlockDeviceManagerEvent::BlockDeviceDisconnected: {
        const QString devUUID = args.value(BlockDeviceManagerEventsArgs::UUID).toString();
        const QString devName = args.value(BlockDeviceManagerEventsArgs::DEV).toString();
        if (devUUID.isEmpty() && devName.isEmpty()) {
            return;
        }

        emit controllerEvent(ControllerEvent::BlockDeviceDisconnected, args);
        qDebug() << "BlockDeviceDisconnected";

        // Если есть подключённое программное хранилище
        if (mSoftwareStorage && (devUUID == mSoftwareStorage->getUUID() || devName == mSoftwareStorage->getDevPath())) {
            softwareStorageDisconnected();
        }
        break;
    }
    case BlockDeviceManagerEvent::BlockDeviceChanged: {
        const QString devUUID = args.value(BlockDeviceManagerEventsArgs::UUID).toString();
        const QString devName = args.value(BlockDeviceManagerEventsArgs::DEV).toString();
        if (devUUID.isEmpty() && devName.isEmpty()) {
            return;
        }

        emit controllerEvent(ControllerEvent::BlockDeviceChanged, args);
        qDebug() << "BlockDeviceChanged";

        // Если есть подключённое программное хранилище
        if (mSoftwareStorage && (devUUID == mSoftwareStorage->getUUID() || devName == mSoftwareStorage->getDevPath())) {
            softwareStorageChanged();
        }
        break;
    }
    }
}
void MonitorController::processAverageSensorValue()
{
    mFileController->writeAverageICPSensorData(mAverageICPController->lastAverageValueByPointer());
    emit dataReadyFromAverageICPController();
}

QString MonitorController::getFlashDevice(QString pathPartDevice)
{
    // Список со всеми подключёнными блочными устройствами
    QList<std::shared_ptr<BlockDevice>> blockDevices;

    // Получаем все блочные устройства в формате json
    QJsonDocument blockDevsJson = QJsonDocument::fromJson(executeAConsoleCommand("bash", QStringList() << "-c" << "lsblk -J -o TYPE,UUID,PATH").toLocal8Bit());
    if (blockDevsJson.isNull()) {
        return {};
    }
    //qDebug() << blockDevsJson;

    // Парсим блочные устройства
    QJsonArray data = blockDevsJson.object().value("blockdevices").toArray();
    QString diskName; // Основное имя диска
    for (const QJsonValue &dev : qAsConst(data))
    {
        const QString blockDevType = dev.toObject().value("type").toString().simplified();
        const QString uuid = dev.toObject().value("uuid").toString().simplified();
        const QString pathDev = dev.toObject().value("path").toString().simplified();

        if ((blockDevType == "part")&&(pathDev == pathPartDevice))// "/dev/sdc1"))
        {
            return uuid;
        }

        //if (getBlockDeviceByUUID(uuid)) {
        //    qDebug() << "The same UUID, IGNORED!";
        //    continue;
        //}

        //// Если это часть основного диска
        //if (devName.contains(diskName)) {
        //}

        //blockDevices.append(std::make_shared<BlockDevice>(uuid, devName));
    }

    return "";
}

void MonitorController::initSoftwareStorage()
{
    qDebug() << "Init Software Storage";
    if (!mICPSettings || !mBlockDeviceManager)
    {
        runState(State::DeinitSoftwareStorage);
        return;
    }

#ifdef PC_BUILD
    QString devUUID = getFlashDevice("/dev/sdc1");
#else
    QString devUUID = getFlashDevice("/dev/sda1");
#endif

    qDebug() << "devUUID" <<devUUID;
    if (devUUID == "")
    {
        qDebug() << "!!!! Add our storage !!!!";
        exit(5);
    }
    if (mICPSettings->getSoftwareStorageUUID() != devUUID)
    {
        qDebug() << "!!!! F L A S H CHANGED !!!!";
    }
    mICPSettings->setSoftwareStorageUUID(devUUID);
    mICPSettings->writeGeneralSettings();

    const std::shared_ptr<BlockDevice> &blockDev = mBlockDeviceManager->getBlockDeviceByUUID(mICPSettings->getSoftwareStorageUUID());

    // Если программное хранилище уже инициализировано

    if (isInitSoftwareStorage())
    {
        softwareStorageAvailable();
        runState(State::UpdateDataBase);
        runState(State::UpdateFileController);
        return;
    }
    // Получаем UUID программного хранилища
    const QString BlockDevUUID = mICPSettings->getSoftwareStorageUUID();

    if (BlockDevUUID.isEmpty())
    {
        qDebug() << "F L A S H K A not INIT";
        softwareStorageNotAssigned();
        runState(State::DeinitDataBase);
        runState(State::UpdateFileController);
        return;
    }
    // Запрашиваем программное хранилище
    //const std::shared_ptr<BlockDevice> &blockDev = mBlockDeviceManager->getBlockDeviceByUUID(BlockDevUUID);
    if (!blockDev || !blockDev->isValid()) { runState(State::DeinitSoftwareStorage); return; }
    // Монтируем хранилище в цикле событий менеджера блочных устройств
    QTimer::singleShot(0, mBlockDeviceManager, [this, blockDev]()
    {
        bool isMnt = blockDev->isMounted();
        if (!isMnt)
        {
            qDebug() << "not mounted";
            isMnt = mBlockDeviceManager->mountBlockDevice(blockDev);
        }
        qDebug() << "isMnt" << isMnt;
        // Смонтировалось ли хранилище
        if (isMnt)
        {
            mSoftwareStorage = blockDev;
            softwareStorageAvailable();
            runState(State::InitDataBase);
            runState(State::UpdateFileController);
        }
        else
        {
            ++mCurrentNumMountSoftwareStorage;
            if (mCurrentNumMountSoftwareStorage < MaxTryMountSoftwareStorage)
            {
                QTimer::singleShot(mIntervalMountSoftwareStorageMs, this, [this]() { runState(State::InitSoftwareStorage); });
            }
            else
            {
                runState(State::DeinitSoftwareStorage);
            }
        }
    });
}

