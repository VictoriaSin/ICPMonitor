#include "blockdevicemanager.h"
#include "blockdevice.h"
#include "deviceWatcher/qdevicewatcher.h"
#include "global_functions.h"
#include <QDebug>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

const QString BlockDeviceManager::BlockDevicesPathLinuxByUUID = "/dev/disk/by-uuid";

BlockDeviceManager::BlockDeviceManager(QObject *parent) : QObject{parent}
{
    // Запускаем слежение за подключемыми/отключаемыми блочными устройствами
    mBlockDeviceWatcher = new QDeviceWatcher(this);
    mBlockDeviceWatcher->appendEventReceiver(this);
    mBlockDeviceWatcher->start();

    // Получаем все установленные устройства в системе
    mAvailableBlockDevices = getBlockDevicesInLinux();
}

BlockDeviceManager::~BlockDeviceManager()
{
    terminate();
}

void BlockDeviceManager::terminate()
{
    qDebug() << "BlockDeviceManager::terminate()";
    if (mBlockDeviceWatcher) {
        mBlockDeviceWatcher->stop();
        delete mBlockDeviceWatcher;
        mBlockDeviceWatcher = nullptr;
    }

    // Сбрасываем все блочные устройства
    for (auto &dev : mAvailableBlockDevices) {
        dev.reset();
    }
    mAvailableBlockDevices.clear();

}

QList<std::shared_ptr<BlockDevice>> BlockDeviceManager::getAvailableBlockDevices() const
{
    return mAvailableBlockDevices;
}

QStringList BlockDeviceManager::getSupportedFileSystems() const
{
    return SupportedFileSystems;
}

bool BlockDeviceManager::mountBlockDevice(const std::shared_ptr<BlockDevice> &blockDevice)
{
    qDebug() << "isContainsBlockDev(blockDevice)" <<isContainsBlockDev(blockDevice);
    return isContainsBlockDev(blockDevice) ? blockDevice->mount() : false;
}

bool BlockDeviceManager::umountBlockDevice(const std::shared_ptr<BlockDevice> &blockDevice)
{
    return isContainsBlockDev(blockDevice) ? blockDevice->umount() : false;
}

std::shared_ptr<BlockDevice> BlockDeviceManager::getBlockDeviceByUUID(const QString &devUUID) const
{
    auto res = std::find_if(mAvailableBlockDevices.begin(), mAvailableBlockDevices.end(), [&devUUID](const std::shared_ptr<BlockDevice> &dev){
        return dev->getUUID() == devUUID;
    });

    if (res == mAvailableBlockDevices.end()) {
        return {};
    }

    return mAvailableBlockDevices.at(std::distance(mAvailableBlockDevices.begin(), res));
}

std::shared_ptr<BlockDevice> BlockDeviceManager::getBlockDeviceByName(const QString &devName) const
{
    auto res = std::find_if(mAvailableBlockDevices.begin(), mAvailableBlockDevices.end(), [&devName](const std::shared_ptr<BlockDevice> &dev){
        return dev->getDevPath() == devName;
    });

    if (res == mAvailableBlockDevices.end()) {
        return {};
    }

    return mAvailableBlockDevices.at(std::distance(mAvailableBlockDevices.begin(), res));
}

void BlockDeviceManager::refreshBlockDevices()
{
    for (const auto &dev : qAsConst(mAvailableBlockDevices)) {
        dev->refresh();
    }
}

bool BlockDeviceManager::isContainsBlockDev(const std::shared_ptr<BlockDevice> &blockDev)
{
    auto res = std::find_if(mAvailableBlockDevices.begin(), mAvailableBlockDevices.end(), [&blockDev](const std::shared_ptr<BlockDevice> &myDev){
        return myDev.get() == blockDev.get();
    });

    if (res == mAvailableBlockDevices.end()) {
        return false;
    }

    return true;
}

void BlockDeviceManager::addBlockDevice(const QVariantMap &blockDevProps)
{
    // Если пришла пустая карта свойств блочного устройства
    if (blockDevProps.isEmpty()) {
        return;
    }

    // Получаем свойства блочного устройства
    const QString Name = blockDevProps.value(BlockDeviceInfo::DEVNAME).toString();
    const QString UUID = blockDevProps.value(BlockDeviceInfo::ID_FS_UUID).toString();
    const QString Vendor = blockDevProps.value(BlockDeviceInfo::ID_VENDOR).toString();
    const QString Model = blockDevProps.value(BlockDeviceInfo::ID_MODEL).toString();
    const QString FileSystem = blockDevProps.value(BlockDeviceInfo::ID_FS_TYPE).toString();
    if (UUID.isEmpty() || Name.isEmpty() || FileSystem.isEmpty()) {
        return;
    }

    // Если в списке доступных уже есть устройство с таким же UUID
    if (getBlockDeviceByUUID(UUID)) {
        qDebug() << "The same UUID, IGNORED!";
        return;
    }

    // Если недопустимая файловая система
    if (!SupportedFileSystems.contains(FileSystem)) {
        return;
    }

    // Добавляем устройство в общий список
    mAvailableBlockDevices.append(std::make_shared<BlockDevice>(UUID, Name, Vendor, Model));

    // Сообщаем об успешном подключении устройства
    emit blockDevicesControllerEvent(BlockDeviceManagerEvent::BlockDeviceConnected,
                                     QVariantMap{{BlockDeviceManagerEventsArgs::UUID, UUID},
                                                 {BlockDeviceManagerEventsArgs::DEV, Name}});

//#ifdef QT_DEBUG
//    // Вывод подключенных устройств
//    coutBlockDevices();
//#endif
}

void BlockDeviceManager::removeBlockDevice(const QVariantMap &blockDevProps)
{
    // Если пришла пустая карта свойств блочного устройства
    if (blockDevProps.isEmpty()) {
        return;
    }

    // Получаем свойства блочного устройства
    const QString Name = blockDevProps.value(BlockDeviceInfo::DEVNAME).toString();
    const QString UUID = blockDevProps.value(BlockDeviceInfo::ID_FS_UUID).toString();

    // Ищем блочное устройство
    std::shared_ptr<BlockDevice> blockDev;
    if (!UUID.isEmpty()) {
        blockDev = getBlockDeviceByUUID(UUID);
    }

    // Если еще не нашли, ищем по имени
    if (!blockDev && !Name.isEmpty()) {
        blockDev = getBlockDeviceByName(Name);
    }

    // Если в итоге не нашли
    if (!blockDev) {
        return;
    }

    // Сбрасываем
    blockDev->reset();

    // Удаляем из списка
    mAvailableBlockDevices.removeOne(blockDev);

    // Сообщаем об отключении устройства
    emit blockDevicesControllerEvent(BlockDeviceManagerEvent::BlockDeviceDisconnected,
                                     QVariantMap{{BlockDeviceManagerEventsArgs::UUID, UUID},
                                                 {BlockDeviceManagerEventsArgs::DEV, Name}});

//#ifdef QT_DEBUG
//    // Вывод подключенных устройств
//    coutBlockDevices();
//#endif
}

void BlockDeviceManager::changeBlockDevice(const QVariantMap &blockDevProps)
{
    // Если пришла пустая карта свойств блочного устройства
    if (blockDevProps.isEmpty()) {
        return;
    }

    // Получаем свойства блочного устройства
    const QString Name = blockDevProps.value(BlockDeviceInfo::DEVNAME).toString();
    const QString UUID = blockDevProps.value(BlockDeviceInfo::ID_FS_UUID).toString();

    // Ищем блочное устройство
    std::shared_ptr<BlockDevice> blockDev;
    if (!UUID.isEmpty()) {
        blockDev = getBlockDeviceByUUID(UUID);
    }

    // Если еще не нашли, ищем по имени
    if (!blockDev && !Name.isEmpty()) {
        blockDev = getBlockDeviceByName(Name);
    }

    // Если в итоге не нашли
    if (!blockDev) {
        return;
    }

    // Обновляем
    blockDev->refresh();

    // Сообщаем об изменении устройства
    emit blockDevicesControllerEvent(BlockDeviceManagerEvent::BlockDeviceChanged,
                                     QVariantMap{{BlockDeviceManagerEventsArgs::UUID, UUID},
                                                 {BlockDeviceManagerEventsArgs::DEV, Name}});

//#ifdef QT_DEBUG
//    // Вывод подключенных устройств
//    coutBlockDevices();
//#endif
}

bool BlockDeviceManager::event(QEvent *ev)
{
    if (ev->type() == QDeviceChangeEvent::registeredType()) {
        QDeviceChangeEvent *event = dynamic_cast<QDeviceChangeEvent *>(ev);

        switch (event->action()) {
        case QDeviceChangeEvent::Add: {
            // Если подсистема блочных устройств
            if (event->subsystem() == QDeviceChangeEvent::Subsystem::Block) {
                addBlockDevice(event->properties());
            }
            break;
        }
        case QDeviceChangeEvent::Change: {
            // Если подсистема блочных устройств
            if (event->subsystem() == QDeviceChangeEvent::Subsystem::Block) {
                changeBlockDevice(event->properties());
            }
            break;
        }
        case QDeviceChangeEvent::Remove: {
            // Если подсистема блочных устройств
            if (event->subsystem() == QDeviceChangeEvent::Subsystem::Block) {
                removeBlockDevice(event->properties());
            }
            break;
        }
        }

        event->accept();
    }

    return QObject::event(ev);
}

void BlockDeviceManager::coutBlockDevices() const
{
    qDebug() << "=======Devices=======";
    for (const auto &dev : qAsConst(mAvailableBlockDevices)) {
        qDebug() << dev->getDevPath() << "mounted to" << dev->getLastMountPoint();
    }
}

QList<std::shared_ptr<BlockDevice>> BlockDeviceManager::getBlockDevicesInLinux()
{
    // Список со всеми подключёнными блочными устройствами
    QList<std::shared_ptr<BlockDevice>> blockDevices;

    // Получаем все блочные устройства в формате json
    QJsonDocument blockDevsJson = QJsonDocument::fromJson(executeAConsoleCommand("bash", QStringList() << "-c" << "lsblk -J -o TYPE,UUID,FSTYPE,PATH,VENDOR,MODEL").toLocal8Bit());
    if (blockDevsJson.isNull()) {
        return {};
    }

    // Парсим блочные устройства
    QJsonArray data = blockDevsJson.object().value("blockdevices").toArray();
    QString diskName; // Основное имя диска
    QString diskVendor; // Производитель диска
    QString diskModel; // Модель диска
    for (const QJsonValue &dev : qAsConst(data))
    {
        const QString blockDevType = dev.toObject().value("type").toString().simplified();
        const QString uuid = dev.toObject().value("uuid").toString().simplified();
        const QString fsType = dev.toObject().value("fstype").toString().simplified();
        const QString devName = dev.toObject().value("path").toString().simplified();
        QString vendor = dev.toObject().value("vendor").toString().simplified();
        QString model = dev.toObject().value("model").toString().simplified();

        // Если диск
        if (blockDevType == "disk") {
            diskName = devName;
            diskVendor = vendor;
            diskModel = model;
        }

        // Если не диск и не раздел, или отсутствует UUID или FSTYPE или имя блочного устройства
        if ((blockDevType != "disk" && blockDevType != "part") || uuid.isEmpty() || fsType.isEmpty() || devName.isEmpty()) {
            continue;
        }

        // Если в списке доступных уже есть устройство с таким же UUID
        if (getBlockDeviceByUUID(uuid)) {
            qDebug() << "The same UUID, IGNORED!";
            continue;
        }

        // Если это часть основного диска
        if (devName.contains(diskName)) {
            vendor = diskVendor;
            model = diskModel;
        }

        // Если недопустимая файловая система
        if (!SupportedFileSystems.contains(fsType)) {
            continue;
        }

        blockDevices.append(std::make_shared<BlockDevice>(uuid, devName, vendor, model));
    }

    return blockDevices;
}
