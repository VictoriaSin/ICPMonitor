#ifndef READER_H
#define READER_H

#include <QFile>
#include "controller/fileProcessing/binfileofindications.h"

class IDataSerializer;
class AbstractICPDataSerializer;
class ComplexValue;

struct ReaderInfo {
    /*! Имя читаемого файла */
    QString nameOfReadingFile;
    /*! Номер читаемого файла */
    int indexReadFileNumber = 0;
    /*! Всего доступных файлов для чтения */
    int numberOfFilesToRead = 0;
    /*! Список испорченных файлов */
    QStringList corruptedFiles;
};

class IReader
{
public:
    virtual bool read(IDataSerializer &serializer) = 0;
    /*! Чтец проинициализирован? */
    virtual bool isInit() const = 0;
    /*! Чтение закончилось? */
    virtual bool atEnd() const = 0;
    /*! Состояние чтеца */
    virtual const ReaderInfo &currentState() const = 0;
};

/*! Чтец файлов
    При работе с чтецом необходимо:
    1) Указать либо путь к файлам и их формат, либо передать
    список с файлами;
    2) Отфильтровать список в соответствии с необходимыми данными;
    3) Вызывать метод read(IDataSerializer *serializer) для чтения в
    данных при помощи установленного serializer.
*/
class Reader : public IReader
{
    /*! Состояние ридера */
    ReaderInfo mReadInfo;


    /*! Фильтрованный список */
    BinFilesList mFilteredListOfBinaries;

    /*! Список файлов для фильтрации */
    BinFilesList mListOfBinariesToFilter;


    /*! Массив байтов из текущего файла */
    QByteArray mFileBytesArray;

    /*! Смещение в байтах для чтения текущего файла */
    int mOffsetBytes = 0;

    /*! Время первого показания (астрономическое) */
    int64_t mAstronomicalFirstSensorReadingTime = 0;

    /*! ID сессии, к которой принадлежит читаемый файл */
    int mFileSessionID = -1;

public:
    Reader();

    /*! Закончились файлы для чтения */
    bool filesEnd() const;
    /*################################################################################
                            Установка папки с бинарными файлами
    ################################################################################*/
    /*! Установка списка с файлами за счет указанного
        пути хранения файлов и их формата
    */
    void setListOfRecordsFiles(const QString &pathForSensorReadings, const QString &formatFile);

    /*! Установка списка с файлами (копирование listOfBinariesToFilter)
        Если список не отсортирован по возрастанию времени,
        то он будет отсортирован.
    */
    void setListOfRecordsFiles(const BinFilesList &listOfBinariesToFilter, bool isSortedByAscendingTime);
    /*################################################################################
                            Отбор необходимых файлов для чтения
    ################################################################################*/
    /*! Фильтрация по указанному поледнему абсолютному времени в миллисекундах.
        recentTime - последнее интересующее абсолютное время (Например: данные за последние 12 часов)
     */
    void filtrationByRecentTime(int64_t recentTime);

    /*! Фильтрация по указанному интервалу времени (timestamp) */
    void filtrationByTimeInterval(int64_t intervalStartTime, int64_t intervalEndTime);

    /*! Фильтрация по указанному интервалу времени (timestamp) и сессии */
    void filtrationByTimeIntervalAndSession(int64_t intervalStartTime, int64_t intervalEndTime, int64_t sessionID);

    /*! Фильтрация бинарных файлов по сессии */
    void filtrationBySession(int64_t sessionID);
    /*################################################################################
                                        Чтение
    ################################################################################*/
    /*! Вычитываем все показания датчика
        result - контейнер, в который будут вычитаны все данные;
        serializer - сериализатор для чтения данных.
    */
    void readAll(QVector<ComplexValue> &result, AbstractICPDataSerializer &serializer);
    /*################################################################################
                                        Сброс чтеца
    ################################################################################*/
    /*! Сброс фильтра */
    void resetFilter();

    /*! Сброс информации о чтении */
    void resetReadInfo();

private:
    /*! Читает следующий файл в фильтрованном списке
        true - следующий файл прочитался;
        false - файлы закончились.
    */
    bool readNextFile();

    /*! Читает текущий файл
        binFileInfo - информация о бинарном файле с показаниями;
        result - сюда считывает файл
    */
    bool readFile(const BinFileOfIndications &binFileInfo, QByteArray &result);

    /*! Повреждённый ли файл */
    bool isCorruptedFile(const QByteArray &file, const BinFileOfIndications &binFileInfo) const;

    /*! Меняет сессию, если у файла сменился ID принадлежности к сессии
        false - сессия осталась прежней;
        true - сессия изменилась
    */
    bool changeFileSessionID(int newSessionID);

    // IReader interface
public:
    bool read(IDataSerializer &serializer) override;
    bool isInit() const override;
    bool atEnd() const override;
    const ReaderInfo &currentState() const override;
};

#endif // READER_H
