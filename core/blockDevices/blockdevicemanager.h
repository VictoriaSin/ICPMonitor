#ifndef BLOCKDEVICEMANAGER_H
#define BLOCKDEVICEMANAGER_H

#include <QObject>
#include <QList>
#include <QThread>
#include <QStorageInfo>
#include <QVariantMap>

#include <memory>

#include "blockdevicemanager_enums.h"

class QDeviceWatcher;
class BlockDevice;

/*! Менеджер всех подключенных блочных устройств
    1) Следит за подключёнными к системе блочными устройствами
    с допустимыми программой файловыми системами;
    2) Предоставляет доступ к блочным устройствам;
    3) Монтирует и размонтирует блочные устройства.

    При обнаружении блочного устройства с UUID,
    которое уже есть в списке достуных,
    игнорирует его.
*/
class BlockDeviceManager : public QObject
{
    Q_OBJECT

    /*! Наблюдатель за блочными устройствами */
    QDeviceWatcher *mBlockDeviceWatcher {nullptr};

    /*! Поддериживаемые файловые системы */
    const QStringList SupportedFileSystems {

#ifdef PC_BUILD
        "ext4",
        "ext3",
        "ext2",
#endif
        "ntfs",
        "vfat"
    };

public:
    /*! Путь к блочным устройствам по uuid */
    static const QString BlockDevicesPathLinuxByUUID;

private:
    /*! Доступные блочные устройства */
    QList<std::shared_ptr<BlockDevice>> mAvailableBlockDevices;

public:
    explicit BlockDeviceManager(QObject *parent = nullptr);
    ~BlockDeviceManager();

    void terminate();

    /*! Возвращает список доступных блочных устройств */
    QList<std::shared_ptr<BlockDevice>> getAvailableBlockDevices() const;

    /*! Возвращает поддерживаемые файловые системы */
    QStringList getSupportedFileSystems() const;

    /*! Поиск блочного устройства по UUID */
    std::shared_ptr<BlockDevice> getBlockDeviceByUUID(const QString &devUUID) const;

    /*! Поиск блочного устройства по имени */
    std::shared_ptr<BlockDevice> getBlockDeviceByName(const QString &devName) const;

    /*! Монтирует блочное устройство */
    bool mountBlockDevice(const std::shared_ptr<BlockDevice> &blockDevice);

    /*! Размонтирует блочное устройство */
    bool umountBlockDevice(const std::shared_ptr<BlockDevice> &blockDevice);

    /*! Обновление всех блочных устройств */
    void refreshBlockDevices();

private:
    /*! Проверка, содержится ли это блочное устройство в
        списке доступных у BlockDeviceManager
    */
    bool isContainsBlockDev(const std::shared_ptr<BlockDevice> &blockDev);

    /*! Добавление блочного устройства */
    void addBlockDevice(const QVariantMap &blockDevProps);

    /*! Удаление блочного устройства */
    void removeBlockDevice(const QVariantMap &blockDevProps);

    /*! Изменение блочного устройства */
    void changeBlockDevice(const QVariantMap &blockDevProps);

    /*! Вывод в консоль устройств в системе */
    void coutBlockDevices() const;

    /*! Возвращает блочные устройства, установленные в Linux
        Получение устройств достигается за счёт парсинга json ответа
        утилиты lsblk -J -o TYPE,UUID,FSTYPE,PATH,VENDOR,MODEL
        Не быстрая функция, рекомендуется применять её лишь при
        создании BlockDeviceManager
    */
    QList<std::shared_ptr<BlockDevice>> getBlockDevicesInLinux();

signals:
    void blockDevicesControllerEvent(BlockDeviceManagerEvent event, const QVariantMap &args = {});

    // QObject interface
public:
    bool event(QEvent *ev) override;
};

#endif // BLOCKDEVICEMANAGER_H
