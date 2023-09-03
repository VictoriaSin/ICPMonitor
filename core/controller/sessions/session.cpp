#include "session.h"
#include "controller/databasemanager.h"

#include <QDebug>

Session::Session(DataBaseManager *dataBase) :
    mDB(dataBase)
{

}

bool Session::setSessionInfo(const QString &infoText)
{
    // В локальную сессию нельзя установить инфомацию
    if (mSessionID == LOCAL_SESSION_ID) {
        return false;
    }

    // Если БД не существует и ID не положительный
    if (!mDB || mSessionID < 1) {
        return false;
    }

    // Если запрос не удачный
    if (!mDB->updateSessionInfo(mSessionID, infoText)) {
        return false;
    }

    // Устанавливаем новую информацию
    mInfo = infoText;

    return true;
}

bool Session::closeSession()
{
    // Устанавливаем время закрытия сессии
    mTimeStampEnd = QDateTime::currentSecsSinceEpoch();

    // Если сессия локальная
    if (mSessionID == LOCAL_SESSION_ID) {
        return true;
    }

    // Если БД не существует и ID не положительный
    if (!mDB || mSessionID < 1) {
        return false;
    }

    // Добавляем в БД время окончания текущей сессии
    //if (!mDB->updateEndTimeSession(mSessionID, mTimeStampEnd)) { mTimeStampEnd = -1; return false; }
// !!!!
    return true;
}

int Session::getId() const
{
     return mSessionID;
}

const int64_t &Session::getTimeStartSession() const
{
    return mTimeStampStart;
}

const int64_t &Session::getTimeEndSession() const
{
    return mTimeStampEnd;
}

const QString &Session::getInfoSession() const
{
    return mInfo;
}

QDebug operator<<(QDebug debug, const Session &val)
{
    debug << "Session(ID:" << val.getId() << ", StartTime:" << val.getTimeStartSession()
          << ", EndTime:" << val.getTimeEndSession() << ", Info:" << val.getInfoSession() << ')';

    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const std::shared_ptr<Session> &val) {
    if (!val) {
        return debug << "Null reference!";
    }
    return debug << *val.get();
}
