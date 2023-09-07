#ifndef FILECONTROLLER_EVENTS_H
#define FILECONTROLLER_EVENTS_H

#include <QObject>

namespace keyfilecontrollerevent {
    constexpr static char dataType[] = "DT";
    constexpr static char transferredFiles[] = "TF";
    constexpr static char countFiles[] = "CF";
    constexpr static char untransferredFiles[] = "UF";
    constexpr static char transferredFileName[] = "TFN";
    constexpr static char errorMessage[] = "MSG";
}

enum FileControllerEvent {
    ExportProgress = 0,     ///< Прогресс экспорта
    ExportDone = 1,         ///< Результат экспорта
    ExportError = 2,        ///< Ошибка экспорта
    ScreenWritten = 3,      ///< Скриншот записан
    ScreenNotWritten = 4,   ///< Скриншот не записан
    FatalWriteCurrentSensorReadingsError = 5, ///< Фатальная ошибка записи текущих показаний (очередь была переполнена)
    FatalWriteAverageSensorReadingsError = 6  ///< Фатальная ошибка записи средних показаний (очередь была переполнена)
};

Q_DECLARE_METATYPE(FileControllerEvent);

#endif // FILECONTROLLER_EVENTS_H
