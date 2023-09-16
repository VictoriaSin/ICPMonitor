#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <memory>

#include <QObject>

#include "global_enums.h"
#include "controller/fileProcessing/export_enums.h"
#include "controller/controller_enums.h"
#include "controller/fileProcessing/filecontroller_events.h"
#include "controller/fileProcessing/binfileofindications.h"

class BlockDevice;
class LabelManager;
class Settings;
class OutputICPSerializer;
class AverageICPSerializer;
class AbstractICPDataSerializer;
class Exporter;
class CycleFileWriter;
class Reader;
class IReader;
class Label;
class ScreenWriter;
struct ComplexValue;
struct ExportResult;

class FileController : public QObject
{
    Q_OBJECT

    /*! ID текущей сессии */
    int mCurrentSessionID = -1;

    /*################################################################################
                                Настройки записи файлов
      ###############################################################################*/
    /*! Максимальное время записи текущих показаний датчика в 1 файл в миллисекундах
        Время в диапазоне от 0 до 65535
     */
    const int mMaxRecordTimeForCSRFileMs = 60000;
    /*! Максимальное время записи средних значений датчика в 1 файл в секундах
        Время в диапазоне от 0 до 65535
     */
    const int mMaxRecordTimeForASRFileSec = 300;

    /*! Максимальная очередь записи RSR файлов */
    const int MaxQueueCSRFile {5};

    /*! Максимальная очередь записи ASR файлов */
    const int MaxQueueASRFile {5};


    /*! Программное хранилище */
    std::shared_ptr<BlockDevice> mSoftwareStorage;

    /*! Формат бинарного файла показаний датчика
        csr - current sensor readings
     */
    const QString FormatOfCurrentSensorReadings {"csr"};

    /*! Формат бинарного файла показаний датчика
        asr - average sensor readings
     */
    const QString FormatOfAverageSensorReadings {"asr"};

    /*! Шаблон имени экспортируемых показаний датчика */
    static const QString TemplateNameOfExportCurrentSensorReadings;

    /*! Шаблон имени экспортируемых средних значений датчика */
    static const QString TemplateNameOfExportAverageSensorReadings;

public:
    explicit FileController(Settings *settings, LabelManager *labelManager, QObject *parent = nullptr);

    void terminate();

    /*! Устанавливает новое программное хранилище */
    void setSoftwareStorage(const std::shared_ptr<BlockDevice> &softwareStorage);

    /*! Устанавливает id сессии */
    void setSessionID(int sessionID);

    /*################################################################################
                                    Обработка скриншотов
      ###############################################################################*/
    /*! Запись скриншота из Linux Frame Buffer (/dev/fb0)
        Будет работать только если видеокарта рисует в него
    */
    void writeScreenFromLinuxFB();

    /*! Запись скриншота из виджета */
    void writeScreenFromWidget(const QPixmap &screen);

    /*! Загружает указанное кол-во скриншотов с устройства на флешку.
        Если указанное кол-во фото больше, чем находится на устройства,
        то загружаются все фотографии с устройства.
        Максимальное кол-во фотографий задано в Setting - mMaxPictures.

        Параметры:
        format - формат конвертирования изображения
        countTransScreenshoots - количество фотографий, необходимых для экспорта
    */
    void exportScreenshots(ConversionPictureFormat format, int countTransScreenshoots);
    /*################################################################################
                            Обработка текущих значений датчика
      ###############################################################################*/
    /*! Запись показания датчика */
    void writeICPSensorData(ComplexValue *sensorData);

    /*! Экспорт последних текущих показаний датчика за указанное время в Мс
        exportPath - путь экспорта;
        exportTimeMs - время необходимого экспорта в миллисекундах;
        exportValues - значения, необходимые для экспорта;
        dateConverterFunc - функция для конвертации даты.
     */
    void exportLastAbsoluteTimeCurrentSensorReadingsMs(const QString &exportPath, uint64_t exportTimeMs,
                                                       uint32_t exportValues, ExportDataFunc func);

    /*! Генерация имени текущих экспортируемых значений датчика */
    static inline QString genExportNameOfCurrentSensorReadings();
    /*################################################################################
                            Обработка средних значений датчика
      ###############################################################################*/
    /*! Запись среднего значения датчика */
    //void writeAverageICPSensorData(ComplexValue *sensorData);

    /*! Экспорт последних средних показаний датчика за указанное время в секундах
        exportPath - путь экспорта;
        exportTimeSec - время необходимого экспорта в секундах;
        exportValues - значения, необходимые для экспорта;
        dateConverterFunc - функция для конвертации даты.
     */
    void exportLastAbsoluteTimeAverageSensorReadingsSec(const QString &exportPath, uint64_t exportTimeSec,
                                                        uint32_t exportValues, ExportDataFunc func);

    /*! Возвращает отсортированный по увеличению времени контейнер средних значений, попадающих в указанный интервал
        intervalStartTimeSeconds - начало интервала средних значений;
        intervalEndTimeSeconds - конец интервала средних значений.
    */
    QVector<ComplexValue> getAverageSensorReadingsFromTheInterval(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds);

    /*! Возвращает отсортированный по увеличению времени контейнер средних значений, попадающих в указанный интервал
        и принадлежащих указанной сессии
        intervalStartTimeSeconds - начало интервала средних значений;
        intervalEndTimeSeconds - конец интервала средних значений;
        sessionID - id интересующей сессии
    */
    QVector<ComplexValue> getAverageSensorReadingsFromTheIntervalForTheSession(uint64_t intervalStartTimeSeconds, uint64_t intervalEndTimeSeconds, uint64_t sessionID);

    /*! Возвращает отсортированный по увеличению времени контейнер средних значений, относящихся к указанной сессии */
    QVector<ComplexValue> getAverageSensorReadingsFromTheSession(uint64_t sessionID);

    /*! Генерация имени средних экспортируемых значений датчика */
    static inline QString genExportNameOfAverageSensorReadings();

private:
    /*! Обновляет максимальное кол-во скриншотов для писателя скриншотов */
    void updateMaxScreensForScreenWriter();

    /*! Отправка события результата экспорта
        exportResult - результат экспорта;
        dataType - наименование экспортируемых данных. (Например: Изображения)
     */
    //void sendEventExportResult(const ExportResult &exportResult, const QString &dataType);

    /*! Экспорт последних показаний датчика.
        serializer - необходимый десериализатор;
        dataPath - путь к папке с записанными бинарными данными соответствующими сериализатору;
        formatFile - расширение бинарного файла;
        dataType - наименование экспортируемых данных (Например: Изображения);
        exportPath - путь для экспорта;
        funcGenName - функция генерации имени файла;
        exportValues - значения, необходимые для экспорта;
        exportTime - время необходимого экспорта.
     */
    //void exportLastAbsoluteTimeSensorReadings(AbstractICPDataSerializer *deserializer, const QString &dataPath,
    //                                          const QString &formatFile, const QString &dataType, const QString &exportPath,
    //                                          const std::function<QString()> &funcGenName,
    //                                          uint32_t exportValues,
    //                                          ExportDataFunc func,
    //                                          uint64_t exportTime);

public slots:
    /*! Обработка событий контроллера */
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

private:
    Settings *mSettings {nullptr};

    /*################################################################################
                                    Писатель скриншотов
      ###############################################################################*/
    /*! Писатель скриншотов */
    //ScreenWriter *mScreenWriter {nullptr};
    /*################################################################################
                            Обработка пришедших значений датчика
      ###############################################################################*/
    /*! Сериализатор пришедших показаний датчика */
    OutputICPSerializer *mOutputICPSerializer {nullptr};

    /*! Писатель пришедших показаний датчика */
    //CycleFileWriter *mCSRFileWriter {nullptr};
    /*################################################################################
                            Обработка средних значений датчика
      ###############################################################################*/
    /*! Сериализатор средних значений датчика */
    AverageICPSerializer *mAverageICPSerializer {nullptr};

    /*! Писатель средних значений датчика */
    CycleFileWriter *mASRFileWriter {nullptr};
    /*################################################################################
                                        Экспорт
      ###############################################################################*/
    /*! Чтец сохранённых показаний датчика */
    Reader *mFileReader {nullptr};

    /*! Экспортёр файлов */
    //Exporter *mFileExporter {nullptr};

    /*! Поток для экспорта */
    QThread *mExportThread {nullptr};

    /*! Десериализатор текущих значений датчика для экспорта */
    OutputICPSerializer *mCSRDeserializer {nullptr};

    /*! Десериализатор средних значений датчика для экспорта */
    AverageICPSerializer *mASRDeserializer {nullptr};

private slots:
    /*! Принимает сигналы прогресса экспорта */
    void exporterProgress(int numberExportedFile, int countOfExportedFiles, QString fileName);

    /*! Результат записи скриншота */
    void screenWrittenResult(bool isWriten);

    /*! Очередь записи текущих показаний переполнена */
    void queueCSRFull();

    /*! Очередь записи средних показаний переполнена */
    void queueASRFull();

signals:
    void fileControllerEvent(FileControllerEvent event, const QVariantMap &args = {});
};

#endif // FILECONTROLLER_H
