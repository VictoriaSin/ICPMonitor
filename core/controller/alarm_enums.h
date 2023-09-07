#ifndef ALARM_ENUMS_H
#define ALARM_ENUMS_H

#include <QObject>

enum AlarmEvent {
    None = 0,   ///< None - Состояние деинициализировано
    Low = 1,    ///< Low - Давления ниже допустимого нижнего порога
    Normal = 2, ///< Normal - Давления в норме
    High = 3    ///< High - Давления выше допустимого верхнего порога
};

Q_DECLARE_METATYPE(AlarmEvent);

#endif // ALARM_ENUMS_H
