#ifndef SESSION_H
#define SESSION_H

#define LOCAL_SESSION_ID 0

#include <memory>

#include <QString>

class SessionManager;
class DataBaseManager;

/*! Сессии
    К сессиям имеют полный доступ
    Менеджер сессий и Менеджер базы данных.
    Остальные могут изменять лишь информацию
    о сессии.
*/
class Session
{
    /*! Информация хранящаяся в БД */
    //int mSessionID {-1};
    //int64_t mTimeStampStart {-1};
    //int64_t mTimeStampEnd {-1};
    //QString mInfo;

    /*! Доступ к базе данных */
    //DataBaseManager* mDB {nullptr};

private:
    /*! Закрытие сессии
        false - не удалось записать в БД;
        true - успешное закрытие
     */
    //bool closeSession();

public:
    explicit Session(DataBaseManager *dataBase);

    /*! Установка информации о сессии */
    //bool setSessionInfo(const QString &infoText);

    /*! Возвращает id сессии */
    //int getId() const;

    /*! Возвращает время начала сессии */
    //const int64_t &getTimeStartSession() const;

    /*! Возвращает время окончания сессии */
    //const int64_t &getTimeEndSession() const;

    /*! Возвращает информацию о сессии */
    //const QString &getInfoSession() const;

    //friend class SessionManager;
    //friend class DataBaseManager;
};

//QDebug operator<<(QDebug debug, const Session &val);
//QDebug operator<<(QDebug debug, const std::shared_ptr<Session> &val);

#endif // SESSION_H
