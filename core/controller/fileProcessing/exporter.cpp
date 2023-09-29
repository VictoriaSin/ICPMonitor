#include "exporter.h"
#include "serialization/abstracticpdataserializer.h"
#include "controller/fileProcessing/reader.h"
#include "controller/labels/label.h"
#include "controller/labels/labelmanager.h"
#include "controller/labels/labelprovider.h"
#include "global_structs.h"
#include "controller/screenwriter.h"

#include <QDateTime>
#include <QDebug>

//Exporter::Exporter(LabelManager *labelManager, QObject *parent) : QObject{parent},
//      mLabelManager(labelManager)
//{
//    // Инициализируем буфер для строки экспорта
//    //mStrBuffer = new char[mMaxStrSize];
//}

//ExportResult Exporter::exportSensorReadings(AbstractICPDataSerializer *deserializer, IReader *reader,
//                                            uint32_t exportValues, ExportDataFunc func,
//                                            const QString &exportPath, const QString &fileName)
//{
//    // Сюда будет записан результат экспорта
//    ExportResult exportResult;

//    // Валидные ли данные пришли
//    if (!isValidData(deserializer, reader, exportPath, fileName, exportValues, &exportResult)) {
//        return exportResult;
//    }

//    // Подготавливаем десериализатор
//    ComplexValue initValue;
//    deserializer->sensorData = &initValue;

//    // Экспортируем в CSV файл
//    exportToCSV(deserializer, reader, exportValues, func, exportPath, fileName, &exportResult);

//    return exportResult;
//}

////void Exporter::exportScreens(ConversionPictureFormat format, int countTransScreenshoots)
////{
//////    QDir screenshotsDir = getPictureDir();
////
//////    // Получаем скриншоты
//////    auto screenshots = screenshotsDir.entryInfoList();
//////    int countScreenshots = screenshots.size();
//////    QStringList corruptedFiles;
////
//////    // Если изображения в папке отсутствуют
//////    if (countScreenshots < 1) {
//////        emit controllerEvent(ControllerEvent::ExportError,QVariantMap({
//////                     {keyfilecontrollerevent::errorMessage, tr("Скриншоты отсутствуют.")}}));
//////        return;
//////    }
////
//////    // Создаем базовую папку для хранения фотографий, если её не существует на флешке
//////    const QString pathToPictureDirOnUsb = Settings::BasePathForMount + '/' + mICPSettings->mNameOfPictureDir;
//////    if (!makeDir(pathToPictureDirOnUsb)) {
//////        return;
//////    }
////
//////    // Если указанное кол-во скриншотов больше, чем находится в папке
//////    if (countTransScreenshoots > countScreenshots) {
//////        countTransScreenshoots = countScreenshots;
//////    }
////
//////    // Перемещаем необходимое кол-во скриншотов
//////    for (int i = 0; i < countTransScreenshoots; ++i) {
//////        // Рассылаем прогресс экспорта
//////        emit controllerEvent(ControllerEvent::ExportProgress, QVariantMap{{keyfilecontrollerevent::transferredFileName, screenshots[i].fileName()},
//////                                                                          {keyfilecontrollerevent::transferredFiles, i},
//////                                                                          {keyfilecontrollerevent::countFiles, countTransScreenshoots}});
//////        // Если скриншот не переместился
//////        if (!exportLastScreenshot(screenshots[i], format, pathToPictureDirOnUsb)) {
//////            corruptedFiles.append(screenshots[i].fileName());
//////        }
//////    }
////
//////    // Сообщаем об окончании экспорта
//////    emit controllerEvent(ControllerEvent::ExportDone, QVariantMap{{keyfilecontrollerevent::dataType, tr("Изображения")},
//////                                                                  {keyfilecontrollerevent::transferredFiles, countTransScreenshoots - corruptedFiles.size()},
//////                                                                  {keyfilecontrollerevent::countFiles, countTransScreenshoots},
//////                                                                  {keyfilecontrollerevent::untransferredFiles, corruptedFiles}});
////
////}

//bool Exporter::exportLastScreenshot(const QFileInfo &fileInfo, ConversionPictureFormat format, const QString &pathToUsbDir) const
//{
//    // Если просят переместить не файл
//    if (!fileInfo.isFile()) {
//        return false;
//    }

//    // Если не пришёл путь экспорта
//    if (pathToUsbDir.isEmpty()) {
//        return false;
//    }

//    // Экспортируем в соответствии с указанным форматом
//    switch (format) {
//    case ConversionPictureFormat::NONE: {
//        return QFile::copy(fileInfo.absoluteFilePath(), pathToUsbDir + fileInfo.fileName());
//    }
//    case ConversionPictureFormat::PNG: {
//        return ScreenWriter::convertAndMovePicture(fileInfo.absoluteFilePath(), pathToUsbDir, ConversionPictureFormat::PNG);
//    }
//    case ConversionPictureFormat::JPEG: {
//        return ScreenWriter::convertAndMovePicture(fileInfo.absoluteFilePath(), pathToUsbDir, ConversionPictureFormat::JPEG);
//    }
//    }

//    return false;
//}

//void Exporter::exportToCSV(AbstractICPDataSerializer *deserializer, IReader *reader,
//                           uint32_t exportValues, ExportDataFunc func, const QString &exportPath,
//                           const QString &fileName, ExportResult *exportResult)
//{
////    // Валидные ли данные пришли
////    if (!isValidData(deserializer, reader, exportPath, fileName, exportValues, exportResult)) {
////        return;
////    }

////    // Устанавливаем имя экспортируемого файла
////    mExportFile.setFileName(exportPath + "/" + fileName + ".csv");

////    // Открылся ли файл
////    if (!mExportFile.open(QIODevice::WriteOnly)) {
////        qDebug() << "ExportFileDidNotOpen";
////        exportResult->result = ExportResult::Result::ExportFileDidNotOpen;
////        return;
////    }

////    // Устанавливаем наименования столбцов и пишем в файл
////    mOffsetStrBuffer = mStrBuffer;
////    memset(mStrBuffer, 0, mMaxStrSize);
////    genColumnName(exportValues, mOffsetStrBuffer);
////    mExportFile.write(mStrBuffer);

////    // Обнуляем буфер данных
////    memset(mStrBuffer, 0, mMaxStrSize);
////    mOffsetStrBuffer = mStrBuffer;

////    // Для рассылки прогресса экспорта и замера времени экспорта
////    mStartExportTime = std::chrono::high_resolution_clock::now();
////    mExportedIndexFile = -1;

////    // Подготавливаем объект провайдера меток
////    std::shared_ptr<LabelProvider> labelProvider;

////    // Для расчёта периода между метками
////    int64_t prevTimeStampMS = 0;

////    // Для хранения показания датчика
////    int64_t sensorTimeStampMS {};

////    // Если пользователь хочет экспорта меток
////    if (((exportValues & ExportValues::UserLabel) == ExportValues::UserLabel ||
////            (exportValues & ExportValues::UserInfoLabel) == ExportValues::UserInfoLabel) &&
////            mLabelManager) {
////        labelProvider = mLabelManager->getLabelProvider();
////    }

////    // Вычитываем всего чтеца и пишем в файл
////    while (!reader->atEnd()) {
////        if (!reader->read(*deserializer)) {
////            continue;
////        }

////        // Если был подготовлен провайдер меток для экспорта
////        if (labelProvider) {
////            // Чистим контейнер меток от предыдущих
////            mLabelContainer.clear();

////            // Узнаём время показания датчика
////            sensorTimeStampMS = deserializer->sensorData->timestamp;

////            // Если период между показаниями уже был рассчитан
////            if (prevTimeStampMS != 0) {
////                labelProvider->getLabelsOnlyForward(deserializer->sessionID, sensorTimeStampMS,
////                                                    sensorTimeStampMS - prevTimeStampMS, &mLabelContainer);
////            } else {
////                labelProvider->getLabelsOnlyForward(deserializer->sessionID, sensorTimeStampMS,
////                                                    prevTimeStampMS, &mLabelContainer);
////            }

////            // Записываем время показания датчика в предыдущее время
////            prevTimeStampMS = sensorTimeStampMS;
////        }

////        // Упаковка данных в строку в соответствии с пользовательскими параметрами
////        dataPackaging(deserializer, exportValues, &mLabelContainer, func, mOffsetStrBuffer);

////        // Запись в файл
////        mExportFile.write(mStrBuffer);

////        // Обнуляем буфер данных
////        memset(mStrBuffer, 0, mMaxStrSize);
////        mOffsetStrBuffer = mStrBuffer;

////        // Рассылка прогресса экспорта
////        const auto &readerState = reader->currentState();
////        if (mExportedIndexFile != readerState.indexReadFileNumber) {
////            sendProgress(readerState);
////            mExportedIndexFile = readerState.indexReadFileNumber;
////        }
////    }

////    // Время затраченное на экспорт
////    qDebug() << "Export time:" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - mStartExportTime).count();

////    // Дописываем буфер в файл и закрываем его
////    mExportFile.flush();
////    mExportFile.close();

////    // Записываем результат экспорта
////    const auto &readerState = reader->currentState();
////    exportResult->countExportedFiles = readerState.indexReadFileNumber - readerState.corruptedFiles.size();
////    exportResult->allCountExportedFiles = readerState.numberOfFilesToRead;
////    exportResult->corruptedFiles = readerState.corruptedFiles;
////    exportResult->result = ExportResult::Result::Done;
//}

//void Exporter::dataPackaging(const AbstractICPDataSerializer *deserializer, uint32_t exportValues,
//                             QVector<Label *> *labelContainer, ExportDataFunc func, char *dst)
//{
//    // Если пришли не валидные данные
//    if (!deserializer || !dst) {
//        return;
//    }

//    // Добавляем дату
//    dst += dateConverterFunc(func, deserializer->sensorData->timestamp, dst);

//    // Добавляем значение
//    *dst = ',';
//    ++dst;
//    dst += i32toa_countlut(deserializer->sensorData->value * 100, dst); // Смещаем плавающую запятую (Надо как-то регулировать в сериализаторе)

//    // Добавляем валидность
//    if ((exportValues & ExportValues::Validity) == ExportValues::Validity) {
//        *dst = ',';
//        ++dst;
//        *dst = deserializer->sensorData->valid ? '1' : '0';
//        ++dst;
//    }

//    // Добавляем состояние тревоги
//    if ((exportValues & ExportValues::AlarmState) == ExportValues::AlarmState) {
//        *dst = ',';
//        ++dst;
//        *dst = deserializer->alarmState ? '1' : '0';
//        ++dst;
//    }

//    // Узнаем, нужно ли нам брать метки
//    bool userLabel = (exportValues & ExportValues::UserLabel) == ExportValues::UserLabel;
//    bool userInfoLabel = (exportValues & ExportValues::UserInfoLabel) == ExportValues::UserInfoLabel;
//    if ((userLabel || userInfoLabel) && labelContainer) {
//        // Количество меток
//        const int countLabels = mLabelContainer.size();

//        // Добавляем пользовательскую метку
//        if (userLabel && countLabels != 0) {
//            *dst = ',';
//            ++dst;

//            // Добавляем все метки
//            for (int i = 0; i < countLabels; ++i) {
//                // Если метка не NULL
//                if (Label *label = mLabelContainer.at(i)) {
//                    dst += i32toa_countlut(label->getNumberLabel(), dst);

//                    // Если есть еще метки
//                    if (i + 1 < countLabels) {
//                        *dst = ';';
//                        ++dst;
//                    }
//                }
//            }
//        }

//        // Добавляем информацию о метке
//        if (userInfoLabel && countLabels != 0) {
//            *dst = ',';
//            ++dst;

//            // Добавляем все метки
//            for (int i = 0; i < countLabels; ++i) {
//                // Если метка не NULL
//                if (Label *label = mLabelContainer.at(i)) {
//                    // Перемещаем информацию о метке
//                    const QByteArray &&bytesInfo = label->getInfoLabel().toLocal8Bit();
//                    if (bytesInfo.size() != 0) {
//                        qstrcpy(dst, bytesInfo.constData());
//                        dst += bytesInfo.size();
//                    }

//                    // Если есть еще метки
//                    if (i + 1 < countLabels) {
//                        *dst = ';';
//                        ++dst;
//                    }
//                }
//            }
//        }
//    }

//    *dst = '\r';
//    dst += 1;
//    *dst = '\n';
//}

//void Exporter::genColumnName(uint32_t exportValues, char *dst)
//{
//    // Если пришли не валидные данные
//    if (!dst) {
//        return;
//    }

//    QByteArray bytesInfo;

//    // Добавляем дату
//    bytesInfo = "Date";
//    qstrcpy(dst, bytesInfo.constData());
//    dst += bytesInfo.size();

//    // Добавляем показание
//    *dst = ',';
//    ++dst;
//    bytesInfo = "ICP";
//    qstrcpy(dst, bytesInfo.constData());
//    dst += bytesInfo.size();

//    // Добавляем валидность
//    if ((exportValues & ExportValues::Validity) == ExportValues::Validity) {
//        *dst = ',';
//        ++dst;
//        bytesInfo = "Data Validity";
//        qstrcpy(dst, bytesInfo.constData());
//        dst += bytesInfo.size();
//    }

//    // Добавляем состояние тревоги
//    if ((exportValues & ExportValues::AlarmState) == ExportValues::AlarmState) {
//        *dst = ',';
//        ++dst;
//        bytesInfo = "Alarm State";
//        qstrcpy(dst, bytesInfo.constData());
//        dst += bytesInfo.size();
//    }

//    // Добавляем состояние тревоги
//    if ((exportValues & ExportValues::UserLabel) == ExportValues::UserLabel) {
//        *dst = ',';
//        ++dst;
//        bytesInfo = "Label Number";
//        qstrcpy(dst, bytesInfo.constData());
//        dst += bytesInfo.size();
//    }

//    // Добавляем состояние тревоги
//    if ((exportValues & ExportValues::UserInfoLabel) == ExportValues::UserInfoLabel) {
//        *dst = ',';
//        ++dst;
//        bytesInfo = "Label Information";
//        qstrcpy(dst, bytesInfo.constData());
//        dst += bytesInfo.size();
//    }

//    *dst = '\r';
//    dst += 1;
//    *dst = '\n';
//}

//bool Exporter::isValidData(AbstractICPDataSerializer *deserializer, IReader *reader, const QString &exportPath, const QString &fileName, uint32_t exportValues, ExportResult *exportResult)
//{
//    if (!exportResult) {
//        return false;
//    }

//    if (!deserializer) {
//        qDebug() << "NoDeserializer";
//        exportResult->result = ExportResult::Result::NoDeserializer;
//        return false;
//    }

//    if (!reader) {
//        qDebug() << "NoReader";
//        exportResult->result = ExportResult::Result::NoReader;
//        return false;
//    }

//    if (!reader->isInit()) {
//        qDebug() << "ReaderIsNotInit";
//        exportResult->result = ExportResult::Result::ReaderIsNotInit;
//        return false;
//    }

//    if (exportPath.isEmpty()) {
//        qDebug() << "NoExportPath";
//        exportResult->result = ExportResult::Result::NoExportPath;
//        return false;
//    }

//    if (fileName.isEmpty()) {
//        qDebug() << "NoFileName";
//        exportResult->result = ExportResult::Result::NoFileName;
//        return false;
//    }

//    if (exportValues == 0) {
//        qDebug() << "NoExportValues";
//        exportResult->result = ExportResult::Result::NoExportValues;
//        return false;
//    }

//    return true;
//}

//void Exporter::sendProgress(const ReaderInfo &readerState)
//{
//    //emit sendExportProgress(readerState.indexReadFileNumber, readerState.numberOfFilesToRead, readerState.nameOfReadingFile);
//}
