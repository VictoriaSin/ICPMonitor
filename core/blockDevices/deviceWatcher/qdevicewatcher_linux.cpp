#include "qdevicewatcher.h"
#include "qdevicewatcher_p.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QSocketNotifier>
#include <QDebug>

#define UEVENT_BUFFER_SIZE 2048

enum udev_monitor_netlink_group { UDEV_MONITOR_NONE, UDEV_MONITOR_KERNEL, UDEV_MONITOR_UDEV };

/**
 * Create new udev monitor and connect to a specified event
 * source. Valid sources identifiers are "udev" and "kernel".
 *
 * Applications should usually not connect directly to the
 * "kernel" events, because the devices might not be useable
 * at that time, before udev has configured them, and created
 * device nodes.
 *
 * Accessing devices at the same time as udev, might result
 * in unpredictable behavior.
 *
 * The "udev" events are sent out after udev has finished its
 * event processing, all rules have been processed, and needed
 * device nodes are created.
 **/

bool QDeviceWatcherPrivate::init()
{
    /* Описание сокета-клиента NetLink */
    struct sockaddr_nl snl;
    const int buffersize = 16 * 1024 * 1024;
    int retval;

    /* Настраиваем сокет для прослушки UDEV */
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_groups = UDEV_MONITOR_UDEV;

    /* Получаем дескриптор сокета */
    mNetlinkSocketDescriptor = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    //netlink_socket = socket(PF_NETLINK, SOCK_DGRAM|SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT); //SOCK_CLOEXEC may be not available
    if (mNetlinkSocketDescriptor == -1) {
        qWarning("error getting socket: %s", strerror(errno));
        return false;
    }

    /* Установка протокола и размера буфера приёма */
    setsockopt(mNetlinkSocketDescriptor, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));

    /* Установка локального адреса сокету */
    retval = bind(mNetlinkSocketDescriptor, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));
    if (retval < 0) {
        qWarning("bind failed: %s", strerror(errno));
        close(mNetlinkSocketDescriptor);
        mNetlinkSocketDescriptor = -1;
        return false;
    } else if (retval == 0) { // Успех
        //from libudev-monitor.c
        struct sockaddr_nl _snl;
        socklen_t _addrlen;

        /* Получаем локальный адрес, назначенный ядром, обычно PID */
        _addrlen = sizeof(struct sockaddr_nl);
        retval = getsockname(mNetlinkSocketDescriptor, (struct sockaddr *) &_snl, &_addrlen);
        if (retval == 0)
            snl.nl_pid = _snl.nl_pid; // Индивидуальный адрес сокета netlink
    }

    /* Интегрируем цикл событий UDEV в цикл событий Qt на основе дескриптора */
    mSocketNotifier = new QSocketNotifier(mNetlinkSocketDescriptor, QSocketNotifier::Read, this);
    connect(mSocketNotifier, SIGNAL(activated(int)), SLOT(parseDeviceInfo())); //will always active
    mSocketNotifier->setEnabled(false);

    return true;
}

bool QDeviceWatcherPrivate::isInit()
{
    return mNetlinkSocketDescriptor != -1 && mSocketNotifier;
}

QDeviceWatcherPrivate::~QDeviceWatcherPrivate()
{
    stop();
    close(mNetlinkSocketDescriptor);
    mNetlinkSocketDescriptor = -1;
}

bool QDeviceWatcherPrivate::start()
{
    // Если сокет не инициализировался
    if (!init()) {
        return false;
    }

    // Включаем рассылку уведомлений
    mSocketNotifier->setEnabled(true);

    return true;
}

bool QDeviceWatcherPrivate::stop()
{
    // Если сокет не инициализирован
    if (!isInit()) {
        return true;
    }

    // Удаляем сокет
    if (mSocketNotifier) {
        mSocketNotifier->setEnabled(false);
        delete mSocketNotifier;
        mSocketNotifier = nullptr;
    }

    // Закрываем соединение
    close(mNetlinkSocketDescriptor);
    mNetlinkSocketDescriptor = -1;

    return true;
}

void QDeviceWatcherPrivate::parseDeviceInfo()
{
    QByteArray data;

    // Читаем сокет
    data.resize(UEVENT_BUFFER_SIZE * 2);
    data.fill(0);
    size_t len = read(mSocketNotifier->socket(), data.data(), UEVENT_BUFFER_SIZE * 2);
    data.resize(len);

    // В исходной строке каждая информация разделена 0
    data = data.replace(0, '\n').trimmed();

    // Парсим блочное устройство
    parseBlockDevice(data);
}

void QDeviceWatcherPrivate::parseBlockDevice(const QByteArray &blockDevInfo)
{
    if (!blockDevInfo.contains("SUBSYSTEM=block")) {
        return;
    }

    // Информация о блочном устройстве
    QList<QByteArray> infoBlockDevice = blockDevInfo.split('\n');

    // Информация, которую ожидаем получить
    QString action {};          // Произошедшее действие
    QString devName {};         // Путь к блочному устройству
    QString vendor {};          // Производитель
    QString model {};           // Модель блочного устройства
    QString fileSystemUUID {};  // UUID файловой системы
    QString fileSystemType {};  // Тип файловой системы

    // Парсим
    for (const auto &infoLine : qAsConst(infoBlockDevice)) {
        // Парсим произошедшее действие
        if (infoLine.startsWith("ACTION=")) {
            action = infoLine.right(infoLine.length() - 7);
        }

        // Парсим путь к блочному устройству
        if (infoLine.startsWith("DEVNAME=")) {
            devName = infoLine.right(infoLine.length() - 8);
        }

        // Парсим производителя
        if (infoLine.startsWith("ID_VENDOR=")) {
            vendor = infoLine.right(infoLine.length() - 10);
        }

        // Парсим модель блочного устройства
        if (infoLine.startsWith("ID_MODEL=")) {
            model = infoLine.right(infoLine.length() - 9);
        }

        // Парсим UUID файловой системы
        if (infoLine.startsWith("ID_FS_UUID=")) {
            fileSystemUUID = infoLine.right(infoLine.length() - 11);
        }

        // Парсим тип файловой системы
        if (infoLine.startsWith("ID_FS_TYPE=")) {
            fileSystemType = infoLine.right(infoLine.length() - 11);
        }
    }

    // Подготавливаем событие
    QDeviceChangeEvent *event {nullptr};

    // Заполняем карту свойств
    QVariantMap props;
    props.insert(BlockDeviceInfo::DEVNAME, devName);
    props.insert(BlockDeviceInfo::ID_VENDOR, vendor);
    props.insert(BlockDeviceInfo::ID_MODEL, model);
    props.insert(BlockDeviceInfo::ID_FS_UUID, fileSystemUUID);
    props.insert(BlockDeviceInfo::ID_FS_TYPE, fileSystemType);

    // Отправляем событие в зависимости от произошедшего действия
    if (action == QLatin1String("add")) {
        event = new QDeviceChangeEvent(QDeviceChangeEvent::Add, QDeviceChangeEvent::Subsystem::Block, props);
    } else if (action == QLatin1String("remove")) {
        event = new QDeviceChangeEvent(QDeviceChangeEvent::Remove, QDeviceChangeEvent::Subsystem::Block, props);
    } else if (action == QLatin1String("change")) {
        event = new QDeviceChangeEvent(QDeviceChangeEvent::Change, QDeviceChangeEvent::Subsystem::Block, props);
    }

    // Рассылка события всем слушателям
    if (event && !event_receivers.isEmpty()) {
        foreach (QObject *obj, event_receivers) {
            QCoreApplication::postEvent(obj, event, Qt::HighEventPriority);
        }
    }
}
