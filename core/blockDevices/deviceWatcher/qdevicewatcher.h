#ifndef QDEVICEWATCHER_H
#define QDEVICEWATCHER_H

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QVariantMap>

/*! Информационные поля блочных устройств */
namespace BlockDeviceInfo {
    constexpr static char DEVNAME[] = "DN";
    constexpr static char ID_VENDOR[] = "VNDR";
    constexpr static char ID_MODEL[] = "MDL";
    constexpr static char ID_FS_UUID[] = "FSUUID";
    constexpr static char ID_FS_TYPE[] = "FSTYPE";
};

class QDeviceWatcherPrivate;

class QDeviceWatcher : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeviceWatcher)
public:
    explicit QDeviceWatcher(QObject *parent = 0);
    ~QDeviceWatcher();

    /*! Запуск прослушки сокета */
    bool start();

    /*! Остановка прослушки сокета */
    bool stop();

    /*! Запущена ли прослушка сокета */
    bool isRunning() const;

    /*! Добавление получателя уведомлений */
    void appendEventReceiver(QObject *receiver);

signals:
    void deviceAdded(const QString &dev);
    void deviceChanged(const QString &dev);
    void deviceRemoved(const QString &dev);

protected:
    /*! Запущена ли прослушка сокета */
    bool running;

    /*! d-pointer */
    QDeviceWatcherPrivate *d_ptr;
};

class QDeviceChangeEvent : public QEvent
{
public:
    enum Action {
        Add,
        Remove,
        Change
    };

    enum Subsystem {
        Block
    };

public:
    explicit QDeviceChangeEvent(Action action, const Subsystem &subsystem, const QVariantMap &properties);

    /*! Действие */
    Action action() const;

    /*! Подсистема устройства */
    Subsystem subsystem() const;

    /*! Свойства подсистемы устройств */
    QVariantMap properties() const;

    /*! Регаем тип данных */
    static QEvent::Type registeredType()
    {
        static QEvent::Type EventType = static_cast<QEvent::Type>(registerEventType());
        return EventType;
    }

private:
    /*! Действие */
    Action mAction;

    /*! Подсистема устройства */
    Subsystem mSubsystem;

    /*! Карта свойст устройства */
    QVariantMap mProperties;
};

#endif // QDEVICEWATCHER_H
