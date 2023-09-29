#ifndef BINFILEOFINDICATIONS_H
#define BINFILEOFINDICATIONS_H

//#include <QString>
//#include <QFileInfo>

//class BinFileOfIndications;

//typedef QList<BinFileOfIndications> BinFilesList;

///*! Агрегат для хранения информации о записанных файлах на устройстве */
//struct BinFilesInfo {
//    /*! Отсортированный по возрастания список хранимых файлов на блочном устройстве */
//    BinFilesList sortedListOfBinFiles {};

//    /*! Время записи всех файлов */
//    int64_t filesRecordingTime {0};

//    /*! Распаршивание файлов из папки
//        dir - папка с файлами
//     */
//    static BinFilesInfo parseBinFilesFromDir(const QDir &dir);
//};

///*! Бинарный файл показаний датчика
//    Хранит информацию о файле
//    Умеет парсить папки с бинарными файлами
//    Умеет искать необходимые бинарные файлы в папке
//*/
//class BinFileOfIndications
//{
//public:
//    /*! Информация о файле */
//    QFileInfo fileInfo;

//    /*! ID сесси, к которой принадлежат данные */
//    int sessionID = -1;

//    /*! Время первой записи в файл */
//    int64_t firstSensorReadings = 0;

//    /*! Время записи файла */
//    uint16_t recordTime = 0;

//    /*! Контрольная сумма */
//    uint8_t crc = 0;

//public:
//    BinFileOfIndications() = default;

//    BinFileOfIndications(const QString &pathToTheFile, int sessionID, int64_t firstSensorReadings, uint16_t recordTime, uint8_t crc);

//    bool operator==(const BinFileOfIndications &infoBinFile);

//    /*! Валидные ли данные о файле */
//    bool isValid() const;

//    /*! Меньшее значение по времени */
//    static bool firstTimestampSensorReadingLessThan(const BinFileOfIndications &v1, const BinFileOfIndications &v2);

//    /*! Большее значение по времени */
//    static bool firstTimestampSensorReadingGreaterThan(const BinFileOfIndications &v1, const BinFileOfIndications &v2);

//    /*! Распаршивание имени файла.
//        Если информации отсутствует, возвращается пустая структура
//        fileInfo - информация о файле
//     */
//    static BinFileOfIndications parseBinFile(const QFileInfo &fileInfo);

//    /*! Поиск бинарных файлов для указанного интервала O(n) */
//    static void searchBinFilesMatchingATimeInterval(BinFilesList *listOfBinaries, BinFilesList *result, int64_t intervalStartTime, int64_t intervalEndTime);

//    /*! Поиск бинарных файлов за последнее указанное время O(n) */
//    static void searchRecentBinFiles(BinFilesList *reverseSortedListOfBinariesByName, BinFilesList *result, int64_t recentTime);

//    /*! Поиск бинарных файлов для указанной сессии и интервала времени O(n) */
//    static void searchBinFilesMatchingATimeIntervalPerSession(BinFilesList *listOfBinaries, BinFilesList *result, int64_t sessionID, int64_t intervalStartTime, int64_t intervalEndTime);

//    /*! Поиск бинарных файлов за указанную сессию O(n) */
//    static void searchBinFilesPerSession(BinFilesList *listOfBinaries, BinFilesList *result, int64_t sessionID);
//};

#endif // BINFILEOFINDICATIONS_H
