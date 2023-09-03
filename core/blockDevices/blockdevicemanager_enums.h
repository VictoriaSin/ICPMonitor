#ifndef BLOCKDEVICEMANAGER_ENUMS_H
#define BLOCKDEVICEMANAGER_ENUMS_H

#include <QObject>

namespace BlockDeviceManagerEventsArgs {
    constexpr static char UUID[] = "UUID";
    constexpr static char DEV[] = "DEV";
}

/*! События контроллера блочных устройств */
enum class BlockDeviceManagerEvent {
    BlockDeviceConnected,           ///< Подключено блочное устройство
    BlockDeviceDisconnected,        ///< Отключено блочное устройство
    BlockDeviceChanged              ///< Изменение блочного устройства
};

Q_DECLARE_METATYPE(BlockDeviceManagerEvent);

#endif // BLOCKDEVICEMANAGER_ENUMS_H
