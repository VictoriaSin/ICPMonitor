#ifndef QDEVICEWATCHER_P_H
#define QDEVICEWATCHER_P_H

#include <QtCore/qglobal.h>
#include <QtCore/QBuffer>
#include <QtCore/QList>

class QSocketNotifier;
class QDeviceWatcher;
class QDeviceWatcherPrivate : public QObject
{
    Q_OBJECT

    /*! Рассылщик уведомлений, прослушивающий сокет */
    QSocketNotifier *mSocketNotifier;

    /*! Класс, который пользуется текущим классом */
    QDeviceWatcher *mWatcher;

    /*! Дескриптор сокета NETLINK */
    int mNetlinkSocketDescriptor;

public:
    QDeviceWatcherPrivate(QObject *parent = 0) : QObject(parent){}
    ~QDeviceWatcherPrivate();

    void setWatcher(QDeviceWatcher *w) { mWatcher = w; }

    /*! Запуск прослушки сокета */
    bool start();

    /*! Остановка прослушки сокета */
    bool stop();

    /*! Объекты прослушки событий */
    QList<QObject *> event_receivers;

private:
    /*! Инициализация сокета прослушки событий ядра UDEV */
    bool init();

    /*! Инициализирован ли сокет прослушки событий ядра UDEV */
    bool isInit();

    /*! Парсинг строки */
    void parseBlockDevice(const QByteArray &blockDevInfo);

private slots:
    /*! Парсинг информации из дескриптора */
    void parseDeviceInfo();

};

#endif // QDEVICEWATCHER_P_H
