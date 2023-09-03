#include "binfileofindications.h"
#include "controller/sessions/session.h"
#include <QDir>

BinFileOfIndications::BinFileOfIndications(const QString &pathToTheFile, int sessionID, int64_t firstSensorReadings, uint16_t recordTime, uint8_t crc)
{
    this->sessionID = sessionID;
    this->fileInfo.setFile(pathToTheFile);
    this->firstSensorReadings = firstSensorReadings;
    this->recordTime = recordTime;
    this->crc = crc;
}

bool BinFileOfIndications::operator==(const BinFileOfIndications &infoBinFile)
{
    return this->fileInfo.absoluteFilePath() == infoBinFile.fileInfo.absoluteFilePath() &&
            this->sessionID == infoBinFile.sessionID &&
            this->firstSensorReadings == infoBinFile.firstSensorReadings &&
            this->recordTime == infoBinFile.recordTime &&
            this->crc == infoBinFile.crc;
}

bool BinFileOfIndications::isValid() const
{
    return this->fileInfo.isFile() &&
            (this->sessionID == LOCAL_SESSION_ID || this->sessionID > 0) &&
            this->firstSensorReadings != 0 &&
            this->recordTime != 0;
}

bool BinFileOfIndications::firstTimestampSensorReadingLessThan(const BinFileOfIndications &v1, const BinFileOfIndications &v2)
{
    return v1.firstSensorReadings < v2.firstSensorReadings;
}

bool BinFileOfIndications::firstTimestampSensorReadingGreaterThan(const BinFileOfIndications &v1, const BinFileOfIndications &v2)
{
    return v1.firstSensorReadings > v2.firstSensorReadings;
}

BinFileOfIndications BinFileOfIndications::parseBinFile(const QFileInfo &fileInfo)
{
    // Если пришёл не файл
    if (!fileInfo.isFile()) {
        return {};
    }

    // Распаршиваем имя файла
    const QStringList parsedFileName = fileInfo.baseName().split('-');

    // Если в имени файла не содержится именно четыре значения
    if (parsedFileName.size() != 4) {
        return {};
    }

    // Для проверки на корректность преобразования в число
    bool ok1 = false, ok2 = false, ok3 = false, ok4 = false;

    // Заполнение возвращаемой структуры
    BinFileOfIndications infoBinFile;
    infoBinFile.fileInfo.setFile(fileInfo.absoluteFilePath());
    infoBinFile.firstSensorReadings = parsedFileName[0].toLongLong(&ok1);
    infoBinFile.sessionID = parsedFileName[1].toInt(&ok2);
    infoBinFile.recordTime = parsedFileName[2].toUInt(&ok3);
    infoBinFile.crc = parsedFileName[3].toShort(&ok4);

    // Если где-то произошла ошибка преобразования
    if (!ok1 || !ok2 || !ok3 || !ok4) {
        return {};
    }

    return infoBinFile;
}

void BinFileOfIndications::searchBinFilesMatchingATimeInterval(BinFilesList *listOfBinaries, BinFilesList *result, int64_t intervalStartTime, int64_t intervalEndTime)
{
    if (!listOfBinaries || !result || intervalStartTime > intervalEndTime) {
        return;
    }

    for (const auto &binFile : qAsConst(*listOfBinaries)) {
        // Если распаршенное имя не валидное
        if (!binFile.isValid()) {
            continue;
        }

        // Если файл попал в интервал
        if (intervalStartTime <= binFile.firstSensorReadings && binFile.firstSensorReadings <= intervalEndTime) {
            result->push_back(binFile);
        }
    }
}

void BinFileOfIndications::searchRecentBinFiles(BinFilesList *reverseSortedListOfBinariesByName, BinFilesList *result, int64_t recentTime)
{
    if (!reverseSortedListOfBinariesByName || !result || recentTime < 0) {
        return;
    }

    // Счётчик инициализированного времени
    int64_t initializedTime = 0;

    // Отбираем указанное кол-во времени
    for (const auto &binFile : qAsConst(*reverseSortedListOfBinariesByName)) {
        // Если заполнили фильтр необходимым временем
        if (initializedTime - 1 > recentTime) {
            break;
        }

        // Если распаршенное имя валидное
        if (binFile.isValid()) {
            result->push_back(binFile );
            initializedTime += binFile.recordTime + 1;
        }
    }
}

void BinFileOfIndications::searchBinFilesMatchingATimeIntervalPerSession(BinFilesList *listOfBinaries, BinFilesList *result, int64_t sessionID, int64_t intervalStartTime, int64_t intervalEndTime)
{
    if (!listOfBinaries || !result || intervalStartTime > intervalEndTime || sessionID < 1) {
        return;
    }

    for (const auto &binFile : qAsConst(*listOfBinaries)) {
        // Если распаршенное имя не валидное
        if (!binFile.isValid()) {
            continue;
        }

        // Если не интересующая нас сессия
        if (binFile.sessionID != sessionID) {
            continue;
        }

        // Если файл попал в интервал
        if (intervalStartTime <= binFile.firstSensorReadings && binFile.firstSensorReadings <= intervalEndTime) {
            result->push_back(binFile);
        }
    }
}

void BinFileOfIndications::searchBinFilesPerSession(BinFilesList *listOfBinaries, BinFilesList *result, int64_t sessionID)
{
    if (!listOfBinaries || !result || sessionID < 1) {
        return;
    }

    for (const auto &binFile : qAsConst(*listOfBinaries)) {
        // Если распаршенное имя не валидное
        if (!binFile.isValid()) {
            continue;
        }

        // Если интересующая нас сессия
        if (binFile.sessionID == sessionID) {
            result->push_back(binFile);
        }
    }
}

BinFilesInfo BinFilesInfo::parseBinFilesFromDir(const QDir &dir)
{
    BinFilesList recordingFiles;
    uint64_t filesRecordingTime = 0;

    const auto fileInfoList = dir.entryInfoList();

    auto it = fileInfoList.cbegin();
    while (it != fileInfoList.cend()) {
        BinFileOfIndications parsedFile = BinFileOfIndications::parseBinFile(it->absoluteFilePath());

        // Если распаршенное имя валидное
        if (parsedFile.isValid()) {
            recordingFiles.push_back(parsedFile);
            filesRecordingTime += parsedFile.recordTime;
        }

        ++it;
    }

    BinFilesInfo info;
    info.sortedListOfBinFiles = std::move(recordingFiles);
    info.filesRecordingTime = filesRecordingTime;
    return info;
}
