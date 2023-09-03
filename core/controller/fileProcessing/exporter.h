#ifndef EXPORTER_H
#define EXPORTER_H

#include "global_enums.h"
#include "export_enums.h"
#include "controller/fileProcessing/reader.h"

#include <chrono>

#include <QObject>
#include <QFile>

/*! Используется для возврата результата экспорта */
struct ExportResult {
    enum Result {
        NoReader,               ///< Нет чтеца
        ReaderIsNotInit,        ///< Чтец не инициализирован
        NoExportPath,           ///< Не указан путь экспорта
        NoFileName,             ///< Не указано имя файла
        ExportFileDidNotOpen,   ///< Файл для экспорта не открылся
        NoExportValues,         ///< Не установлены экспортируемые значения
        NoDeserializer,         ///< Не установлен десериализатор для данных
        Done                    ///< Экспорт завершён
    };

    /*! Кол-во экспортированных файлов */
    int countExportedFiles = 0;
    /*! Общее кол-во экспортируемых файлов */
    int allCountExportedFiles = 0;
    /*! Список испорченных файлов */
    QStringList corruptedFiles;
    /*! Результат экспорта */
    Result result;

    ExportResult() = default;

    ExportResult(int countExportedFiles, int allCountExportedFiles, QStringList corruptedFiles) {
        this->countExportedFiles = countExportedFiles;
        this->allCountExportedFiles = allCountExportedFiles;
        this->corruptedFiles = corruptedFiles;
    }
};

class IReader;
class AbstractICPDataSerializer;
class AverageICPSerializer;
class OutputICPSerializer;
class LabelManager;
class LabelProvider;
class Label;
class Settings;

class Exporter : public QObject
{
    Q_OBJECT

    /*! Экспортируемый файл */
    QFile mExportFile;

    /*! Менеджер меток */
    LabelManager *mLabelManager {nullptr};

    /*! Контайнер для меток, относящихся к показанию датчика */
    QVector<Label *> mLabelContainer;

    /*! Буфер данных для строки экспорта */
    char *mStrBuffer {nullptr};

    /*! Максимальный размер строки экспорта */
    int mMaxStrSize {1024};

    /*! Для смещения по буферу и его заполнения */
    char *mOffsetStrBuffer {nullptr};

    // Замер скорости экспорта
    /*! Время начала экспорта */
    std::chrono::time_point<std::chrono::system_clock,
    std::chrono::duration<long long,
    std::ratio<1, 1000000000>>> mStartExportTime {};

    /*! Индекс экспортируемого файла */
    int mExportedIndexFile {-1};

public:
    explicit Exporter(LabelManager *labelManager, QObject *parent = nullptr);

    /*! Экспорт средних значений датчика
        serializer - десериализатор данных;
        reader - чтец с предустановленными данными;
        format - формат экспортируемого файла;
        exportPath - путь экспорта;
        fileName - имя экспортируемого файла.
    */
    ExportResult exportSensorReadings(AbstractICPDataSerializer *deserializer, IReader *reader,
                                      uint32_t exportValues, ExportDataFunc func,
                                      const QString &exportPath, const QString &fileName);

    /*! Загружает указанное кол-во скриншотов с устройства на флешку.
            Если указанное кол-во фото больше, чем находится на устройства,
            то загружаются все фотографии с устройства.
            Максимальное кол-во фотографий задано в Setting - mMaxPictures.

            Параметры:
            format - формат конвертирования изображения
            countTransScreenshoots - количество фотографий, необходимых для экспорта
        */
        //void exportScreens(ConversionPictureFormat format, int countTransScreenshoots);

        /*! Загружает последний скриншот на флешку */
        bool exportLastScreenshot(const QFileInfo &fileInfo, ConversionPictureFormat format, const QString &pathToUsbDir) const;

private:
    /*! Экспорт в формате CSV
        1) В ходе экспорта просим reader считать
        данные из файла в предоставленный ему deserializer;
        2) Упаковываем данные, считанные reader-ом в deserializer в строку
        в соответствии с выбранными пользователем экспортируемыми значениями;
        3) Записываем в файл.
    */
    void exportToCSV(AbstractICPDataSerializer *deserializer, IReader *reader,
                     uint32_t exportValues, ExportDataFunc func, const QString &exportPath,
                     const QString &fileName, ExportResult *exportResult);

    /*! Упаковка данных
        deserializer - десериализатор данных;
        exportValues - необходимые для экспорта значения (дубли стоит удалить);
        dst - данные упаковываются в этот пакет
    */
    inline void dataPackaging(const AbstractICPDataSerializer *deserializer, uint32_t exportValues,
                              QVector<Label *> *labelContainer, ExportDataFunc func, char *dst);

    /*! Генерация названий колонок файла
        exportValues - экспортируемые значения;
        dst - сюда формируются названия столбцов
    */
    inline void genColumnName(uint32_t exportValues, char *dst);

    /*! Проверка пришедних данных на валидность */
    bool isValidData(AbstractICPDataSerializer *deserializer, IReader *reader, const QString &exportPath,
                     const QString &fileName, uint32_t exportValues, ExportResult *exportResult);

    /*! Отправка прогресса экспорта */
    inline void sendProgress(const ReaderInfo &readerState);

signals:
    /*! Информирование об экспорте
        numberExportedFile - номер экспортируемого файла
        countOfExportedFiles - кол-во экспортируемых файлов
        fileName - имя экспортируемого файла
     */
    void sendExportProgress(int numberExportedFile, int countOfExportedFiles, QString fileName);
};

#endif // EXPORTER_H
