#include "cyclefilewriter.h"
#include "serialization/abstracticpdataserializer.h"
#include "controller/fileProcessing/filecontroller.h"
#include "controller/sessions/session.h"
#include "controller/fileProcessing/binfileofindications.h"
#include "blockDevices/blockdevice.h"
#include "global_functions.h"

#include <QIODevice>
#include <QDir>
#include <QDebug>
#include <QDateTime>

CycleFileWriter::CycleFileWriter(const QString &pathForSensorReadings, const QString &formatFile, int64_t maxRecordTimeForFile,
                                 int64_t maxRecordingTime, int maxQueueRecordsFile, QObject *parent)
    : QObject{parent},
      RecordingFileFormat(formatFile),
      mRelativePath(pathForSensorReadings),
      mMaxStorageTime(maxRecordingTime),
      mMaxRecordFileTime(maxRecordTimeForFile),
      MaxQueueRecordFiles(maxQueueRecordsFile)
{

}

CycleFileWriter::~CycleFileWriter()
{
    resetBuffer(0);
}

void CycleFileWriter::setSessionID(int sessionID)
{
    mSessionID = sessionID;
}

void CycleFileWriter::terminate()
{
    deinitCycleWriting();
}

void CycleFileWriter::setBlockDevice(const std::shared_ptr<BlockDevice> &blockDev)
{
    mBlockDevice = blockDev;
    deinitFilesDir();
    initFilesDir();
}

bool CycleFileWriter::initCycleWriting()
{
    if (mState != State::WaitInitCycleWriting) {
        return false;
    }

    // Очищаем очередь файлов записи
    mQueueRecordFilesToBlockDev.clear();

    // Переводим в состояние циклической записи данных
    mState = State::CycleWriting;

    return true;
}

void CycleFileWriter::deinitCycleWriting()
{
    return;
    // Добавляем файл в очередь на запись
    if (mRecordFile) {
        moveRecFileToQueue();
    }

    // Записываем очередь на блочное устройство
    writeQueueToBlockDevice();

    // Очищаем очередь файлов записи (если остались)
    mQueueRecordFilesToBlockDev.clear();

    // Переводим в состояние ожидания инициализации циклической записи
    mState = State::WaitInitCycleWriting;
}

bool CycleFileWriter::writeData(const IDataSerializer &serializer)
{
        return true;
    if (!isWriteSessionActive()) {
        return false;
    }

    // Если очередь переполнилась
    if (mQueueRecordFilesToBlockDev.size() >= MaxQueueRecordFiles) {
        mState = State::FullQueue;
        emit queueFull();
        return false;
    }

    // Подготавливаем буффер для новых данных
    resetBuffer(serializer.getMaxReadOrWriteData() + 2);
    if (!mBuffer) {
        return false;
    }

    // Если запись файла закончена
    if (mRecordFile && mRecordFile->isEndRecordFile()) {
        moveRecFileToQueue();
    }

    // Если файла нет
    if (!mRecordFile) {
        mRecordFile = genRecordFile();
    }

    // Записываем время показания
    mRecordFile->writeAbsoluteTime(serializer.getAbsoluteTime());

    // Записываем в буффер время от начала записи файла
    *(uint16_t *)mBuffer = mRecordFile->recTime;

    // Упаковка пришедших данных в строку и запись в файл
    const int bytesWritten = serializer.serializeData(mBuffer + 2);
    if (bytesWritten != 0) {
        mRecordFile->writeData(mBuffer, 2 + bytesWritten);
    }

    // Если время записи файла достигнуто
    if (mRecordFile->recTime >= mMaxRecordFileTime) {
        moveRecFileToQueue();
    }

    // Запись данных на блочное устройство
    writeQueueToBlockDevice();

    return true;
}

const BinFilesList &CycleFileWriter::getSortedListOfRecordsFiles()
{
    initFilesDir();
    return mBinFilesInfoOnBlockDev.sortedListOfBinFiles;
}

void CycleFileWriter::moveRecFileToQueue()
{
    if (!mRecordFile) {
        return;
    }

    mRecordFile->endRecordFile();
    mQueueRecordFilesToBlockDev.enqueue(mRecordFile);
    mRecordFile.reset();
}

std::shared_ptr<CycleFileWriter::RecordFile> CycleFileWriter::genRecordFile() const
{
    auto recFile = std::make_shared<RecordFile>(RecordingFileFormat);
    recFile->sessionID = mSessionID;
    return recFile;
}

bool CycleFileWriter::isWriteSessionActive() const
{
    return mState == State::CycleWriting;
}

void CycleFileWriter::deleteOldFilesFromBlockDevice()
{
    if (!initFilesDir()) {
        return;
    }

    while (mBinFilesInfoOnBlockDev.filesRecordingTime > mMaxStorageTime) {
        auto it = mBinFilesInfoOnBlockDev.sortedListOfBinFiles.begin();
        if (it == mBinFilesInfoOnBlockDev.sortedListOfBinFiles.end()) {
            return;
        }

        // Удаляем файл
        QFile::remove(it->fileInfo.absoluteFilePath());
        if (QFile::exists(it->fileInfo.absoluteFilePath())) {
            return;
        }

        qDebug() << "Delete " << it->fileInfo.absoluteFilePath();
        mBinFilesInfoOnBlockDev.filesRecordingTime -= it->recordTime;
        mBinFilesInfoOnBlockDev.sortedListOfBinFiles.erase(it);
    }
}

void CycleFileWriter::deinitFilesDir()
{
    if (mFilesDir) {
        mFilesDir.reset();
    }
    mBinFilesInfoOnBlockDev = BinFilesInfo();
}

bool CycleFileWriter::initFilesDir()
{
    return false;
    if (!isReadyBlockDev()) {
        mBinFilesInfoOnBlockDev = BinFilesInfo();
        return false;
    }

    if (isReadyFilesDir()) {
        return true;
    }

    deinitFilesDir();

    const QString filesDirPath = mBlockDevice->getLastMountPoint() + '/' + mRelativePath;
    if (makeDir(filesDirPath)) {
        mFilesDir = std::make_unique<QDir>(filesDirPath, "*." + RecordingFileFormat, QDir::Name, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        mBinFilesInfoOnBlockDev = BinFilesInfo::parseBinFilesFromDir(*mFilesDir); // Обновляем информацию о бинарных файлах
    }

    return isReadyFilesDir();
}

bool CycleFileWriter::isReadyFilesDir() const
{
    if (!mFilesDir) {
        return false;
    }

    mFilesDir->refresh();
    return mFilesDir->exists();
}

bool CycleFileWriter::isReadyBlockDev()
{
    if (!mBlockDevice) {
        return false;
    }
    mBlockDevice->refresh();
    return mBlockDevice->isReadyToReadWrite();
}

void CycleFileWriter::writeQueueToBlockDevice()
{
    if (mQueueRecordFilesToBlockDev.isEmpty()) {
        return;
    }

    // Если папка для файлов не создана(лась)
    if (!initFilesDir()) {
        qDebug() << "Queue size:" << mQueueRecordFilesToBlockDev.size();
        return;
    }

    // Кол-во файлов записи
    const int numQueue = mQueueRecordFilesToBlockDev.size();
    int curIndex {0};
    for (int i = 0; i < numQueue; ++i) {
        // Получаем файл записи из очереди
        const auto &recordFile = mQueueRecordFilesToBlockDev.at(curIndex);
        if (!recordFile) {
            mQueueRecordFilesToBlockDev.removeAt(curIndex);
            continue;
        }

        // Записываем в файл на блочном устройстве буфер данных
        const QString AbsoluteFilePath {mFilesDir->absolutePath() + '/' + recordFile->fileName()};
        QFile file(AbsoluteFilePath);
        if (!file.open(QIODevice::WriteOnly)) {
            ++curIndex;
            continue;
        }

        const int64_t time = QDateTime::currentMSecsSinceEpoch();
        file.write(recordFile->buffer);
        file.flush();
        file.close();
        qDebug() << "Time for Write " << AbsoluteFilePath << ":" << QDateTime::currentMSecsSinceEpoch() - time;

        // Если успешно перемещено на блочное устройство
        if (QFile::exists(AbsoluteFilePath)) {
            mBinFilesInfoOnBlockDev.sortedListOfBinFiles.append(BinFileOfIndications(AbsoluteFilePath,
                                                                                     recordFile->sessionID,
                                                                                     recordFile->recStartTimestamp,
                                                                                     recordFile->recTime,
                                                                                     recordFile->crc));
            mBinFilesInfoOnBlockDev.filesRecordingTime += recordFile->recTime;
            mQueueRecordFilesToBlockDev.removeAt(curIndex);
            continue;
        }
        ++curIndex;
    }

    // Удаление старых файлов с блочного устройства
    deleteOldFilesFromBlockDevice();
}

void CycleFileWriter::resetBuffer(const int bufferSize)
{
    // Если изменился размер буффера
    if (mBufferSize != bufferSize) {
        if (mBuffer) {
            delete [] mBuffer;
            mBuffer = nullptr;
        }
        mBufferSize = bufferSize;
    }

    // Выделяем память под буффер
    if (!mBuffer && mBufferSize > 0) {
        mBuffer = new char[mBufferSize];
    }

    // Обнуляем буффер данных
    if (mBuffer) {
        memset(mBuffer, 0, mBufferSize);
    }
}

bool CycleFileWriter::RecordFile::writeAbsoluteTime(const int64_t &absoluteTime)
{
    if (isEndRecordFile()) {
        return false;
    }

    if (state == State::WaitRecord) {
        recStartTimestamp = absoluteTime;
        state = State::Record;
    }

    if (state == State::Record) {
        recTime = absoluteTime - recStartTimestamp;
    }

    return true;
}

bool CycleFileWriter::RecordFile::writeData(const char *data, int len)
{
    if (isEndRecordFile() || !data || len == 0) {
        return false;
    }
    buffer.append(data, len);
    crc = crc8(data, len, crc); // Добавление значения к контрольной сумме файла
    return true;
}

void CycleFileWriter::RecordFile::endRecordFile()
{
    state = State::EndRecord;
}
