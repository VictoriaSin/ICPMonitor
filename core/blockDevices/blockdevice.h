#ifndef BLOCKDEVICE_H
#define BLOCKDEVICE_H

#include "blockdevicemanager.h"
#include "blockdevicemanager_enums.h"

/*! Информация о блочном устройстве
    Для создания блочного устройства
    необходимо передать его идентификатор,
    путь к нему и точку, куда необходимо его монтировать.
*/
class BlockDevice
{
    /*! Потенциальная точка монтирования - путь к папке */
    const QString MountPointDir;

    /*! UUID */
    const QString mUUID;

    /*! Путь к блочному устройству */
    QString mDevName;

    /*! Производитель */
    QString mVendor;

    /*! Модель */
    QString mModel;
public:
    /*! Информация о хранилище */
    QStorageInfo mInfo;
private:
    /*! Флаг валидности устройства */
    bool mIsValid {true};

public:
    /*! Файл смонтированных блочных устройств */
    static const QString MountedBlockDevicesFileName;

    /*! Путь монтирования внешних накопителей */
    static const QString BaseMountPointExternalDevice;

    /*! Возвращает путь монтирования (Парсит MountedBlockDevicesFile)
        devPath - путь к блочному устройству
    */
    static QString mountedPath(const QString &devPath);

public:
    BlockDevice(const QString &UUID, const QString &devName, const QString &vendor, const QString &model);

    ~BlockDevice();

    /*! Валидное (Не излечено) ли устройство */
    bool isValid() const;

    /*! Смонтирован ли диск
        Если необходимо проверить на текущий момент,
        то сначало необходимо сделать refresh()
    */
    bool isMounted() const;

    /*! Готово ли блочное устройство для Чтения/Записи
        Если необходимо проверить на текущий момент,
        то сначало необходимо сделать refresh()
    */
    bool isReadyToReadWrite() const;

    /*! Возвращает уникальный идентификатор блочного устройства */
    QString getUUID() const;

    /*! Возвращает путь к блочному устройству */
    QString getDevPath() const;

    /*! Возвращает производителя блочного устройства */
    QString getVendor() const;

    /*! Возвращает модель блочного устройства */
    QString getModel() const;

    /*! Кэшированая информация о хранилище.
        Если необходимо проверить на текущий момент,
        то сначало нужно сделать refresh().
        Обязательно проверять на валидность
        при использовании!
    */
    QStorageInfo getLastStorageInfo() const;

    /*! Возвращает последний путь монтирования.
        Если необходимо проверить на текущий момент,
        то сначало нужно сделать refresh()
    */
    QString getLastMountPoint() const;

    /*! Обновление текущего устройства */
    void refresh();

private:
    /*! Монтирование диска
        true - успешно смонтировался;
        false - ошибка монтирования.
    */
    bool mount();

    /*! Размонтирование диска
        Размонитрует только те диски,
        которые монтировал сам.
        true - успешно размонтирован;
        false - ошибка размонтирования.
    */
    bool umount();

    /*! Создание папки монтирования */
    bool createMountPointDir();

    /*! Удаление папки монтирования */
    bool removeMountPointDir();

    /*! Сброс блочного устройства
        Менеджер блочных устройств пользуется,
        когда устройство отключается от системы.
    */
    void reset();

    friend BlockDeviceManager;
};

#endif // BLOCKDEVICE_H
