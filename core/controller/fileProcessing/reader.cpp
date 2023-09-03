#include "reader.h"
#include "controller/fileProcessing/filecontroller.h"
#include "controller/fileProcessing/binfileofindications.h"
#include "controller/labels/label.h"
#include "serialization/idataserializer.h"
#include "serialization/abstracticpdataserializer.h"
#include "global_functions.h"
#include "global_structs.h"

#include <QDir>
#include <QDebug>
#include <QThread>

Reader::Reader()
{

}

void Reader::setListOfRecordsFiles(const QString &pathForSensorReadings, const QString &formatFile)
{
    // Парсим названия бинарных файлов из папки
    QDir sensorReadingsDir(pathForSensorReadings, "*." + formatFile, QDir::Name, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    mListOfBinariesToFilter = BinFilesInfo::parseBinFilesFromDir(sensorReadingsDir).sortedListOfBinFiles;
}

void Reader::setListOfRecordsFiles(const BinFilesList &listOfBinariesToFilter, bool isSortedByAscendingTime)
{
    mListOfBinariesToFilter = listOfBinariesToFilter; // Копируем

    // Сортируем по возрастанию
    if (!isSortedByAscendingTime) {
        std::sort(mListOfBinariesToFilter.begin(), mListOfBinariesToFilter.end(), BinFileOfIndications::firstTimestampSensorReadingLessThan);
    }
}

void Reader::filtrationByRecentTime(int64_t recentTime)
{
    resetFilter();

    BinFilesList reverseContainer = reversed(mListOfBinariesToFilter);
    BinFileOfIndications::searchRecentBinFiles(&reverseContainer,
                                               &mFilteredListOfBinaries,
                                               recentTime);

    // Устанавливаем кол-во файлов после фильтрации
    mReadInfo.numberOfFilesToRead = mFilteredListOfBinaries.size();

    // Очищаем список файлов для фильтрации
    mListOfBinariesToFilter.clear();
}

void Reader::filtrationByTimeInterval(int64_t intervalStartTime, int64_t intervalEndTime)
{
    resetFilter();

    BinFileOfIndications::searchBinFilesMatchingATimeInterval(&mListOfBinariesToFilter,
                                                              &mFilteredListOfBinaries,
                                                              intervalStartTime,
                                                              intervalEndTime);

    // Устанавливаем кол-во файлов после фильтрации
    mReadInfo.numberOfFilesToRead = mFilteredListOfBinaries.size();

    // Очищаем список файлов для фильтрации
    mListOfBinariesToFilter.clear();
}

void Reader::filtrationByTimeIntervalAndSession(int64_t intervalStartTime, int64_t intervalEndTime, int64_t sessionID)
{
    resetFilter();

    BinFileOfIndications::searchBinFilesMatchingATimeIntervalPerSession(&mListOfBinariesToFilter,
                                                                        &mFilteredListOfBinaries,
                                                                        sessionID,
                                                                        intervalStartTime,
                                                                        intervalEndTime);

    // Устанавливаем кол-во файлов после фильтрации
    mReadInfo.numberOfFilesToRead = mFilteredListOfBinaries.size();

    // Очищаем список файлов для фильтрации
    mListOfBinariesToFilter.clear();
}

void Reader::filtrationBySession(int64_t sessionID)
{
    resetFilter();

    BinFileOfIndications::searchBinFilesPerSession(&mListOfBinariesToFilter,
                                                   &mFilteredListOfBinaries,
                                                   sessionID);

    // Устанавливаем кол-во файлов после фильтрации
    mReadInfo.numberOfFilesToRead = mFilteredListOfBinaries.size();

    // Очищаем список файлов для фильтрации
    mListOfBinariesToFilter.clear();
}

void Reader::readAll(QVector<ComplexValue> &result, AbstractICPDataSerializer &serializer)
{
    ComplexValue *value = serializer.sensorData;
    if (!value) {
        return;
    }

    // Вычитываем всего чтеца и заполняем контейнер
    while (!atEnd()) {
        if (!read(serializer)) {
            continue;
        }

        result.append(*value);
    }
}

bool Reader::isInit() const
{
    return mListOfBinariesToFilter.size() != 0 || mFilteredListOfBinaries.size() != 0;
}

bool Reader::filesEnd() const
{
    return mReadInfo.indexReadFileNumber >= mReadInfo.numberOfFilesToRead;
}

bool Reader::atEnd() const
{
    return filesEnd() && mFileBytesArray.isEmpty();
}

bool Reader::readNextFile()
{
    // Если файлы закончились
    if (filesEnd()) {
        return false;
    }

    // Информация о бинарном файле
    const BinFileOfIndications &infoFile = mFilteredListOfBinaries.at(mReadInfo.indexReadFileNumber);

    // Если не считался файл
    if (!readFile(infoFile, mFileBytesArray)) {
        // Добавляем в список битых файлов
        mReadInfo.corruptedFiles.append(infoFile.fileInfo.fileName());
        ++mReadInfo.indexReadFileNumber;
        return readNextFile();
    }

    // Если файл битый
    if (isCorruptedFile(mFileBytesArray, infoFile)) {
        // Добавляем в список битых файлов
        mReadInfo.corruptedFiles.append(infoFile.fileInfo.fileName());
        ++mReadInfo.indexReadFileNumber;
        mFileBytesArray.clear(); // Чистим считанный файл
        return readNextFile();
    }

    // Сохраняем имя читаемого файла
    mReadInfo.nameOfReadingFile = infoFile.fileInfo.fileName();

    // Устанавливаем время первого показания
    mAstronomicalFirstSensorReadingTime = infoFile.firstSensorReadings;

    // Меняем сессию, если она изменилась
    changeFileSessionID(infoFile.sessionID);

    // Переключаем на следующий файл
    ++mReadInfo.indexReadFileNumber;

    return true;
}

bool Reader::readFile(const BinFileOfIndications &binFileInfo, QByteArray &result)
{
    // Текущий читаемый файл
    QFile currentReadingFile(binFileInfo.fileInfo.absoluteFilePath());

    // Если файл не открылся для чтения
    if (!currentReadingFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Читаем весь файл
    result = currentReadingFile.readAll();

    // Закрываем файл
    currentReadingFile.close();

    return true;
}

bool Reader::isCorruptedFile(const QByteArray &file, const BinFileOfIndications &binFileInfo) const
{
    // Проверяем, не битый ли файл
    return crc8(file.data(), file.size()) != binFileInfo.crc;
}

bool Reader::read(IDataSerializer &serializer)
{
    if (atEnd()) {
        return false;
    }

    // Обнуляем serializer
    serializer.reset();

    // Считываем следующий файл, если закончился текущий
    if (mFileBytesArray.isEmpty()) {
        // Если файлы закончились
        if (!readNextFile()) {
            return false;
        }
    }

    // Читаем байты времени
    serializer.setAbsoluteTime(mAstronomicalFirstSensorReadingTime + *(uint16_t *)(mFileBytesArray.data() + mOffsetBytes));
    mOffsetBytes += 2;

    // Тут устанавливаем принадлежность к сессии
    serializer.setSesionID(mFileSessionID);

    // Читаем байты при помощи десериализатора
    mOffsetBytes += serializer.deserializeData(mFileBytesArray.constData() + mOffsetBytes);

    // Если вычитали весь файл
    if (mOffsetBytes >= mFileBytesArray.size()) {
        mFileBytesArray.clear();
        mOffsetBytes = 0;
    }

    return true;
}

const ReaderInfo &Reader::currentState() const
{
    return mReadInfo;
}

void Reader::resetFilter()
{
    mFilteredListOfBinaries.clear();
    mFileBytesArray.clear();
    mOffsetBytes = 0;
    mAstronomicalFirstSensorReadingTime = 0;
    resetReadInfo();
}

void Reader::resetReadInfo()
{
    mReadInfo.nameOfReadingFile = "";
    mReadInfo.indexReadFileNumber = 0;
    mReadInfo.numberOfFilesToRead = mFilteredListOfBinaries.size();
    mReadInfo.corruptedFiles.clear();
}

bool Reader::changeFileSessionID(int newSessionID)
{
    // Если и так стоит данная сессия
    if (mFileSessionID == newSessionID) {
        return false;
    }

    // Меняем id сессии
    mFileSessionID = newSessionID;

    return true;
}
