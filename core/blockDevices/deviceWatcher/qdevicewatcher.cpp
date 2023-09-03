#include "qdevicewatcher.h"
#include "qdevicewatcher_p.h"

QDeviceWatcher::QDeviceWatcher(QObject *parent)
    : QObject(parent)
    , running(false)
    , d_ptr(new QDeviceWatcherPrivate(this))
{
    Q_D(QDeviceWatcher);
    d->setWatcher(this);
}

QDeviceWatcher::~QDeviceWatcher()
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = NULL;
    }
}

bool QDeviceWatcher::start()
{
    Q_D(QDeviceWatcher);
    if (!d->start()) {
        stop();
        running = false;
    }
    running = true;
    return running;
}

bool QDeviceWatcher::stop()
{
    Q_D(QDeviceWatcher);
    running = !d->stop();
    return !running;
}

bool QDeviceWatcher::isRunning() const
{
    return running;
}

void QDeviceWatcher::appendEventReceiver(QObject *receiver)
{
    Q_D(QDeviceWatcher);
    d->event_receivers.append(receiver);
}

QDeviceChangeEvent::QDeviceChangeEvent(Action action, const Subsystem &subsystem, const QVariantMap &properties)
    : QEvent(registeredType())
{
    mAction = action;
    mSubsystem = subsystem;
    mProperties = properties;
}

QDeviceChangeEvent::Action QDeviceChangeEvent::action() const
{
    return mAction;
}

QDeviceChangeEvent::Subsystem QDeviceChangeEvent::subsystem() const
{
    return mSubsystem;
}

QVariantMap QDeviceChangeEvent::properties() const
{
    return mProperties;
}
