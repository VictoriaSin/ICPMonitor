#ifndef CYCLEFILEWRITER_H
#define CYCLEFILEWRITER_H

#include <memory>

#include <QObject>
#include <QFileInfo>
#include <QQueue>

#include "global_structs.h"
#include "controller/fileProcessing/binfileofindications.h"

/*! Базовое имя файла записи */
#define RECORDED_FILENAME "RECORD"

class Settings;
class AlarmController;
class IDataSerializer;
class BlockDevice;

/*! Класс, производящий циклическую запись сериализованных
    показаний датчика в файл.

    Файл ограничен временем записи.
    Директория, в которую ведётся запись, ограничивается
    максимальным кол-вом времени записи.

    Конструктор принимает все необходимые параметры для
    корректной работы циклического писателя:
    mPathForSensorReadings - Путь хранения показаний датчика;
    FormatOfRecordFile - Формат файла для показаний датчика (без точки);
    mMaxRecordTimeForFile - Максимальное время записи в файл;
    mMaxRecordingTime - Максимальное время записи всех файлов;

    Выходной формат файлов:
    АбсолютноеВремяПервогоПоказания-IDСессии-КолвоВремениЗаписиВФайле-КонтрСумма.Формат
*/
class CycleFileWriter : public QObject
{
    Q_OBJECT

    /*! Файл записи */
    struct RecordFile {
        enum class State {
            WaitRecord, ///< Ожидает записи
            Record,     ///< Пишется
            EndRecord   ///< Запись окончена
        };

        QByteArray buffer {};                   ///< Буфер с записанными данными
        int sessionID {0};                      ///< ID сессии, к которой принадлежат данные
        int64_t recStartTimestamp {0};          ///< Время первой записи
        int64_t recTime {0};                    ///< Время записи файла
        uint8_t crc {0};                        ///< Контрольная сумма файла
        QString format {};                      ///< Формат файла

        State state {State::WaitRecord};        ///< Состояние записи

        RecordFile(const QString &_formatFile) : format(_formatFile){};

        /*! Записывает данные в файл */
        bool writeData(const char *data, int len);

        /*! Записывает абсолютное время и считает время запси файла */
        bool writeAbsoluteTime(const int64_t &absoluteTime);

        /*! Для принудительного завершения записи файла */
        void endRecordFile();

        /*! Закончилась ли запись в файл */
        bool isEndRecordFile() {return state == State::EndRecord;}

        /*! Генерация имени файла
            sessionID - id сессии, к которой принадлежит файл
            recStartTimestamp - время начала записи файла
            recTime - время записи файла
            crc - контрольная сумма
            formatFile - формат файла (Например: rsr, bin и т.д.)
            Выдаваемый формат: recStartTimestamp-sessionID-recTime-crc.format
        */
        QString fileName() const {
            return QString("%1-%2-%3-%4.%5")
                            .arg(recStartTimestamp)
                            .arg(sessionID)
                            .arg(recTime)
                            .arg(crc)
                            .arg(format);
        }
    };

public:
    /*! Состояние циклического писателя */
    enum class State {
        WaitInitCycleWriting,   ///< Ожидание старта инициализации циклической записи
        CycleWriting,           ///< Циклическая запись данных
        FullQueue               ///< Очередь переполнена
    };

private:
    /*! Текущее состояние писателя */
    State mState {State::WaitInitCycleWriting};

    /*! Формат файлов записи */
    const QString RecordingFileFormat;

    /*! ID сессии */
    int mSessionID {-1};

    /*! Относительный путь хранения файлов записи */
    QString mRelativePath;

    /*! Максимальное кол-во времени хранения */
    int64_t mMaxStorageTime {0};

    /*! Максимальное время записи файла */
    int64_t mMaxRecordFileTime {0};

    /*! Буфер данных */
    char *mBuffer {nullptr};
    int mBufferSize = 0;

    /*! Папка хранения файлов записи
        Меняется при смене блочного
        устройства
    */
    std::unique_ptr<QDir> mFilesDir;

    /*! Блочное устройство, куда производится запись */
    std::shared_ptr<BlockDevice> mBlockDevice;

    /*! Максимальная очередь записанных файлов, ожидающих запись на блочное устройство */
    const int MaxQueueRecordFiles {5};

    /*! Очередь записанных файлов для записи на блочное устройство */
    QQueue<std::shared_ptr<RecordFile>> mQueueRecordFilesToBlockDev;

    /*! Текущий записываемый файл */
    std::shared_ptr<RecordFile> mRecordFile;

    /*! Информация о бинарных файлах на блочном устройстве */
    BinFilesInfo mBinFilesInfoOnBlockDev {};

public:
    explicit CycleFileWriter(const QString &pathForSensorReadings, const QString &formatFile,
                             int64_t maxRecordTimeForFile, int64_t maxRecordingTime,
                             int maxQueueRecordsFile, QObject *parent = nullptr);

    ~CycleFileWriter();

    /*! Принудительное завершение */
    void terminate();

    /*! Установка блочного устройства */
    void setBlockDevice(const std::shared_ptr<BlockDevice> &blockDev);

    /*! Возвращает блочное устройство */
    std::shared_ptr<BlockDevice> getBlockDevice() const { return mBlockDevice; };

    /*! Устанавливает сессию */
    void setSessionID(int sessionID);

    /*! Инициализация циклической записи */
    bool initCycleWriting();

    /*! Деинициализация циклической записи записи */
    void deinitCycleWriting();

    /*! Идёт ли запись в файл */
    bool isWriteSessionActive() const;

    /*! Запись в файл */
    bool writeData(const IDataSerializer &serializer);

    /*! Относительный путь записи файлов
        (Относительно точки монтирования блочного устройства)
    */
    QString getRelativePath() const { return mRelativePath; };

    /*! Возвращает отсортированный по возрастания список записанных файлов
        (Использовать только в рамках одного потока!)
    */
    const BinFilesList &getSortedListOfRecordsFiles();

private:
    /*! Перемещает файл в очередь */
    void moveRecFileToQueue();

    /*! Генерирует новый файл записи */
    std::shared_ptr<RecordFile> genRecordFile() const;

    /*! Удаление старых файлов c блочного устройства */
    void deleteOldFilesFromBlockDevice();

    /*! Инициализация папки для файлов записи
        Если папка создана или существует,
        то возвращается true. Иначе false.
    */
    bool initFilesDir();

    /*! Деинициализация папки файлов записи */
    void deinitFilesDir();

    /*! Готова ли папка для файлов записи */
    bool isReadyFilesDir() const;

    /*! Готово ли блочное устройство к взаимодействию */
    bool isReadyBlockDev();

    /*! Запись файлов из очереди на блочное устройство */
    void writeQueueToBlockDevice();

    /*! Сброс буфера для принимаемых данных
        bufferSize - необходимый размер буфера
        Если размер будет равен нулю, то mBuffer будет nullptr
    */
    void resetBuffer(const int bufferSize);


signals:
    /*! Сигнал, оповещающий, что очередь переполнена */
    void queueFull();
};

#endif // CYCLEFILEWRITER_H
