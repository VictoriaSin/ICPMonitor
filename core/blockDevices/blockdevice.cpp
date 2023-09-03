#include "blockdevice.h"
#include "global_functions.h"

#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QTimer>

const QString BlockDevice::MountedBlockDevicesFileName {"/proc/mounts"};
const QString BlockDevice::BaseMountPointExternalDevice {"/media"};

BlockDevice::BlockDevice(const QString &UUID, const QString &devName, const QString &vendor, const QString &model) :
    MountPointDir(BaseMountPointExternalDevice + "/" + QCoreApplication::applicationName()),/* + "-" + UUID),*/
    mUUID(UUID),
    mDevName(devName),
    mVendor(vendor),
    mModel(model)
{
    refresh();
}

BlockDevice::~BlockDevice()
{
    //reset();
}

bool BlockDevice::isValid() const
{
    return mIsValid;
}

void BlockDevice::refresh()
{
    if (!isValid()) { return; }

    // Узнаём текущую точку монтирования
    const QString mntPoint = mountedPath(mDevName);

    // Обновляем кэшированную информацию
    mInfo = mntPoint.isEmpty() ? QStorageInfo() : QStorageInfo(mntPoint);
}

bool BlockDevice::isMounted() const
{
    return mInfo.isValid();
}

bool BlockDevice::isReadyToReadWrite() const
{
    return isValid() && isMounted();
}

QStorageInfo BlockDevice::getLastStorageInfo() const
{
    return mInfo;
}

QString BlockDevice::getLastMountPoint() const
{
    return mInfo.isValid() ? mInfo.rootPath() : "";
}

QString BlockDevice::getDevPath() const
{
    return mDevName;
}

QString BlockDevice::getVendor() const
{
    return mVendor;
}

QString BlockDevice::getModel() const
{
    return mModel;
}

QString BlockDevice::getUUID() const
{
    return mUUID;
}

bool BlockDevice::mount()
{
    // Если блочное устройство не валидное
    //qDebug() << "Если блочное устройство не валидное";
    if (!isValid()) { return false; }

    // Обновляем
    refresh();
    qDebug() << "isMounted()";
    // Если устройство и так смонтировано
    if (isMounted()) {
        return true;
    }

    // Если программный путь к папке монтирования пустой
    qDebug() << "MountPointDir.isEmpty()";
    if (MountPointDir.isEmpty()) { return false; }

    // Создаём папку для монтирования
    qDebug() << "createMountPointDir";
    if (!createMountPointDir()) {
        return false;
    }

    // Монтируем
    qDebug() << "mount, QStringList() << mDevName << MountPointDir";
    executeAConsoleCommand("mount", QStringList() << mDevName << MountPointDir);
qDebug() << "MountPointDir = " << MountPointDir << " mDevName=" << mDevName;
    // Обновляем
    refresh();

    // Если не смонтировался
    if (!isMounted()) {
        removeMountPointDir();
        return false;
    }

    return isMounted();
}

bool BlockDevice::umount()
{
    // Если программный путь к папке монтирования пустой
    if (MountPointDir.isEmpty()) {
        return false;
    }
    // Размонтируем
    executeAConsoleCommand("umount", QStringList() << MountPointDir);
    // Удаляем папку
    const bool isRmMntDir = removeMountPointDir();
    // Обновляем
    refresh();

    return isRmMntDir;
}

bool BlockDevice::createMountPointDir()
{
    // Если программный путь к папке монтирования пустой
    if (MountPointDir.isEmpty()) { return false; }

//    // Создаём папку монтирования устройства
//    QDir mountDir(MountPointDir);
//    if (!mountDir.exists()) { if (!mountDir.mkdir(mountDir.path())) { return false; } }
//    return mountDir.exists();

    QProcess process;
    process.start("mkdir", QStringList() << "-m" << "777" << MountPointDir);
    process.waitForFinished();
    qDebug() << "normal = "<< process.readAllStandardOutput();
    qDebug() << "error =" << process.readAllStandardError();
    process.close();
    return true;
}

bool BlockDevice::removeMountPointDir()
{
    // Если программный путь к папке монтирования пустой
    if (MountPointDir.isEmpty()) { return false; }
    QDir mountDir(MountPointDir);
    if (mountDir.exists())
    {
        mountDir.rmdir(mountDir.path());
    }
    return !mountDir.exists();
}

void BlockDevice::reset()
{
    // Размонтируем
    umount();
    // Инвалидируем
    mIsValid = false;
}

QString BlockDevice::mountedPath(const QString &devPath)
{
    QFile procMountsFile(MountedBlockDevicesFileName);
    // Если не удалось открыть файлs
    if (!procMountsFile.open(QIODevice::OpenModeFlag::ReadOnly)) { return {}; }
    // Читаем все смонтированные блочные устройства
    QByteArray mountedBlockDevicesFile = procMountsFile.readAll().trimmed();
    // Закрываем файл
    procMountsFile.close();
    // Если файл пустой
    if (mountedBlockDevicesFile.isEmpty()) { return {}; }
    // Поиск точки монтирования
    const QByteArrayList devLines = mountedBlockDevicesFile.split('\n');
    for (const auto &line : devLines)
    {
        const QByteArrayList temp = line.split(' ');
        if (temp.first() == devPath)
        {
            return QString(temp.at(1)).replace("\\040", " ");
        }
    }
    return {};
}
