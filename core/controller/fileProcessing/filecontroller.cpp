#include "filecontroller.h"
#include "global_functions.h"
#include "controller/fileProcessing/exporter.h"
#include "controller/fileProcessing/cyclefilewriter.h"
#include "controller/fileProcessing/reader.h"
#include "controller/settings.h"
#include "serialization/abstracticpdataserializer.h"
#include "controller/labels/labelmanager.h"
#include "controller/screenwriter.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QTextStream>
#include <QThread>
#include <QPixmap>

//const QString FileController::TemplateNameOfExportCurrentSensorReadings {"%1-wave-%2"};
//const QString FileController::TemplateNameOfExportAverageSensorReadings {"%1-trend-%2"};

//FileController::FileController(Settings *settings, LabelManager *labelManager, QObject *parent) :
//    QObject{parent},
//    mSettings(settings)
//{
//    qDebug("!!!Kill me!!!");
//    // Инициализация писателей файлов и скринов
//    //mCSRFileWriter = new CycleFileWriter(mSettings->getRelativeCurrentSensorReadingsPath(), FormatOfCurrentSensorReadings, mMaxRecordTimeForCSRFileMs, mSettings->getMaxTimeStorageCurrentSensorReadingsMs(), MaxQueueCSRFile, this);
//    mASRFileWriter = new CycleFileWriter(mSettings->getRelativeAverageSensorReadingsPath(), FormatOfAverageSensorReadings, mMaxRecordTimeForASRFileSec, mSettings->getMaxTimeStorageAverageSensorReadingsSec(), MaxQueueASRFile, this);
//    mOutputICPSerializer = new OutputICPSerializer;
//    mAverageICPSerializer = new AverageICPSerializer;
//    //mScreenWriter = new ScreenWriter(mSettings->getRelativeScreensPath(), mSettings->getMaxScreens());
//    //connect(mCSRFileWriter, &CycleFileWriter::queueFull, this, &FileController::queueCSRFull);
//    connect(mASRFileWriter, &CycleFileWriter::queueFull, this, &FileController::queueASRFull);

//    // Статус записи скриншота
//    //connect(mScreenWriter, &ScreenWriter::screenWrittenFinished, this, &FileController::screenWrittenResult);

//    // Инициализация экспорта
//    mExportThread = new QThread(this);
//    //mFileExporter = new Exporter(labelManager);
//    mFileReader = new Reader;
//    mCSRDeserializer = new OutputICPSerializer;
//    mASRDeserializer = new AverageICPSerializer;

//    // Прогресс экспорта
//    //connect(mFileExporter, &Exporter::sendExportProgress, this, &FileController::exporterProgress);

//    // Сброс в поток
//    //mScreenWriter->moveToThread(mExportThread);
//    //mFileExporter->moveToThread(mExportThread);
//    mExportThread->start();
//}

//void FileController::terminate()
//{
//    //mCSRFileWriter->terminate();
//    mASRFileWriter->terminate();

//    mExportThread->quit();
//    mExportThread->wait(3000);
//}

//void FileController::setSoftwareStorage(const std::shared_ptr<BlockDevice> &softwareStorage)
//{
//    mSoftwareStorage = softwareStorage;
//    mASRFileWriter->setBlockDevice(softwareStorage);
//    //mCSRFileWriter->setBlockDevice(softwareStorage);

//    //QTimer::singleShot(0, mScreenWriter, [this](){mScreenWriter->setBlockDevice(mSoftwareStorage);});
//}

//void FileController::setSessionID(int sessionID)
//{
//    // Если и так установлена текущая сессия
//    if (mCurrentSessionID == sessionID) {
//        return;
//    }

//    // Устанавливаем текущую сессию
//    mCurrentSessionID = sessionID;

//    // Устанавливаем писателям текущий ID сессии
//    mASRFileWriter->setSessionID(mCurrentSessionID);
//    //mCSRFileWriter->setSessionID(mCurrentSessionID);
//}

//void FileController::writeScreenFromLinuxFB()
//{
//    //QTimer::singleShot(0, mScreenWriter, [screenSaver = this->mScreenWriter](){ screenSaver->writeScreenFromLinuxFB();    });
//}

//void FileController::writeScreenFromWidget(const QPixmap &/*screen*/)
//{
//    //QTimer::singleShot(0, mScreenWriter, [screenWriter = this->mScreenWriter, scr = std::move(screen)](){ screenWriter->writeScreenFromWidget(scr);    });
//}

//void FileController::exportScreenshots(ConversionPictureFormat /*format*/, int /*countTransScreenshoots*/)
//{
//    // Экспортируем скриншоты
////    QTimer::singleShot(0, mFileExporter, [this]() {

////    });
//}

//void FileController::exportLastAbsoluteTimeCurrentSensorReadingsMs(const QString &exportPath, uint64_t exportTimeMs, uint32_t exportValues, ExportDataFunc func)
//{
//    //exportLastAbsoluteTimeSensorReadings(mCSRDeserializer, mSettings->getRelativeCurrentSensorReadingsPath(),
//    //                                     FormatOfCurrentSensorReadings, tr("Текущие показания датчика"),
//    //                                     exportPath, genExportNameOfCurrentSensorReadings, exportValues, func, exportTimeMs);
//}

//void FileController::exportLastAbsoluteTimeAverageSensorReadingsSec(const QString &exportPath, uint64_t exportTimeSec, uint32_t exportValues, ExportDataFunc func)
//{
//    //exportLastAbsoluteTimeSensorReadings(mASRDeserializer, mSettings->getRelativeAverageSensorReadingsPath(),
//    //                                     FormatOfAverageSensorReadings, tr("Средние показания датчика"),
//    //                                     exportPath, genExportNameOfAverageSensorReadings, exportValues, func, exportTimeSec);
//}

//QVector<ComplexValue> FileController::getAverageSensorReadingsFromTheInterval(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds)
//{
//    // Подготавливаем чтеца и фильтруем
//    mFileReader->setListOfRecordsFiles(mASRFileWriter->getSortedListOfRecordsFiles(), true);
//    mFileReader->filtrationByTimeInterval(intervalStartTimeSeconds, intervalEndTimeSeconds);

//    // Подготавливаем контейнер найденных средних значений
//    QVector<ComplexValue> averageSensorReadings;

//    // Подготавливаем десериализатор
//    ComplexValue value;
//    mASRDeserializer->sensorData = &value;

//    // Вычитываем все средние значения
//    mFileReader->readAll(averageSensorReadings, *mASRDeserializer);

//    return averageSensorReadings;
//}

////QVector<ComplexValue> FileController::getAverageSensorReadingsFromTheIntervalForTheSession(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds, uint64_t sessionID)
////{
////    // Подготавливаем чтеца и фильтруем
////    mFileReader->setListOfRecordsFiles(mASRFileWriter->getSortedListOfRecordsFiles(), true);
////    mFileReader->filtrationByTimeIntervalAndSession(intervalStartTimeSeconds, intervalEndTimeSeconds, sessionID);

////    // Подготавливаем контейнер для найденных средних значений
////    QVector<ComplexValue> averageSensorReadings;

////    // Подготавливаем десериализатор
////    ComplexValue value;
////    mASRDeserializer->sensorData = &value;

////    // Вычитываем все средние значения
////    mFileReader->readAll(averageSensorReadings, *mASRDeserializer);

////    return averageSensorReadings;
////}

////QVector<ComplexValue> FileController::getAverageSensorReadingsFromTheSession(uint64_t sessionID)
////{
////    return {};
////}

////void FileController::exportLastAbsoluteTimeSensorReadings(AbstractICPDataSerializer *deserializer, const QString &dataPath,
////                                                          const QString &formatFile, const QString &dataType, const QString &exportPath,
////                                                          const std::function<QString ()> &funcGenName, uint32_t exportValues,
////                                                          ExportDataFunc func, uint64_t exportTime)
////{
////    // Настраиваем чтеца (указываем папку с файлами и их формат. Фильтруем по абсолютному времени.)
////    mFileReader->setListOfRecordsFiles(dataPath, formatFile);
////    mFileReader->filtrationByRecentTime(exportTime);
////
////    // Генерируем имя экспортируемому файлу
////    const QString nameExportFile = funcGenName();
////
////    // Экспортируем данные
////    //QTimer::singleShot(0, mFileExporter, [this, deserializer, dataType, exportPath, exportValues, func, nameExportFile]() {
////    //    struct ExportResult exportResult = this->mFileExporter->exportSensorReadings(deserializer, this->mFileReader, exportValues, func, exportPath, nameExportFile);
//////        sendEventExportResult(exportResult, dataType);
////    //});
////}

////void FileController::sendEventExportResult(const ExportResult &exportResult, const QString &dataType)
////{
////    switch (exportResult.result) {
////    case ExportResult::Result::Done: {
////        emit fileControllerEvent(FileControllerEvent::ExportDone, QVariantMap{{keyfilecontrollerevent::dataType, dataType},
////                                                                              {keyfilecontrollerevent::transferredFiles, exportResult.countExportedFiles},
////                                                                              {keyfilecontrollerevent::countFiles, exportResult.allCountExportedFiles},
////                                                                              {keyfilecontrollerevent::untransferredFiles, exportResult.corruptedFiles}});
////        break;
////    }
////    case ExportResult::Result::ExportFileDidNotOpen: {
////        emit fileControllerEvent(FileControllerEvent::ExportError, QVariantMap{{keyfilecontrollerevent::errorMessage, tr("Не удалось начать запись в файл")}});
////        break;
////    }
////    default:
////        emit fileControllerEvent(FileControllerEvent::ExportError, QVariantMap{{keyfilecontrollerevent::errorMessage, tr("Ошибка экспорта")}});
////    }
////}

//void FileController::writeICPSensorData(ComplexValue *sensorData)
//{
//    mOutputICPSerializer->sensorData = sensorData;
//    //mCSRFileWriter->writeData(*mOutputICPSerializer);
//}

////void FileController::writeAverageICPSensorData(ComplexValue *sensorData)
////{
////    mAverageICPSerializer->sensorData = sensorData;
////    mASRFileWriter->writeData(*mAverageICPSerializer);
////}

//QString FileController::genExportNameOfCurrentSensorReadings()
//{
//    return TemplateNameOfExportCurrentSensorReadings.arg(QCoreApplication::applicationName()).arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
//}

//QString FileController::genExportNameOfAverageSensorReadings()
//{
//    return TemplateNameOfExportAverageSensorReadings.arg(QCoreApplication::applicationName()).arg(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
//}

//void FileController::updateMaxScreensForScreenWriter()
//{
//    if (!mSettings) {
//        return;
//    }

//    //QTimer::singleShot(0, mScreenWriter, [this](){ mScreenWriter->setMaxScreens(mSettings->getMaxScreens());    });
//}

//void FileController::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
//{
//    switch (event) {
//    case ControllerEvent::SessionStarted: {
//        //mCSRFileWriter->initCycleWriting();
//        mASRFileWriter->initCycleWriting();
//        break;
//    }
//    case ControllerEvent::SessionClosed: {
//        //mCSRFileWriter->deinitCycleWriting();
//        mASRFileWriter->deinitCycleWriting();
//        break;
//    }
//    case ControllerEvent::AlarmLowOn: {
//        mOutputICPSerializer->alarmState = true;
//        mAverageICPSerializer->alarmState = true;
//        break;
//    }
//    case ControllerEvent::AlarmOff: {
//        mOutputICPSerializer->alarmState = false;
//        mAverageICPSerializer->alarmState = false;
//        break;
//    }
//    case ControllerEvent::AlarmHighOn: {
//        mOutputICPSerializer->alarmState = true;
//        mAverageICPSerializer->alarmState = true;
//        break;
//    }
//    case ControllerEvent::UpdatedMaxScreens: {
//        updateMaxScreensForScreenWriter();
//        break;
//    }
//    default: break;
//    }
//}

//void FileController::exporterProgress(int numberExportedFile, int countOfExportedFiles, QString fileName)
//{
//    emit fileControllerEvent(FileControllerEvent::ExportProgress, QVariantMap{{keyfilecontrollerevent::transferredFileName, fileName},
//                                                                              {keyfilecontrollerevent::transferredFiles, numberExportedFile},
//                                                                              {keyfilecontrollerevent::countFiles, countOfExportedFiles}});
//}

//void FileController::screenWrittenResult(bool isWriten)
//{
//    if (isWriten) {
//        qDebug() << "Screen Written";
//        emit fileControllerEvent(FileControllerEvent::ScreenWritten);
//    } else {
//        qDebug() << "Screen Not Written";
//        emit fileControllerEvent(FileControllerEvent::ScreenNotWritten, QVariantMap{{GlobalEventsArgs::Message,
//                                                                                     tr("Ошибка!\n"
//                                                                                     "Снимок экрана не сделан!")}});
//    }
//}

//void FileController::queueCSRFull()
//{
//    //mCSRFileWriter->deinitCycleWriting();
//    emit fileControllerEvent(FileControllerEvent::FatalWriteCurrentSensorReadingsError);
//}

//void FileController::queueASRFull()
//{
//    mASRFileWriter->deinitCycleWriting();
//    emit fileControllerEvent(FileControllerEvent::FatalWriteAverageSensorReadingsError);
//}
