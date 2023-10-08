#include "sessionmanager.h"
#include "controller/databasemanager.h"
//#include "controller/sessions/session.h"

SessionManager::SessionManager(DataBaseManager *dataBase)//    : mDataBase(dataBase)
{

}

//const std::shared_ptr<Session> &SessionManager::getCurrentSession() const
//{
//    return mCurrentSession;
//}

//bool SessionManager::isActiveSession() const
//{
//    return mCurrentSession.get();
//}

//QVector<std::shared_ptr<Session>> SessionManager::getAllSession()
//{
//    // Если нет БД
//    if (!mDataBase) {
//        return QVector<std::shared_ptr<Session>>();
//    }

//    return mDataBase->selectAllSessions();;
//}

//void SessionManager::terminate()
//{
//    //closeCurrentSession();
//}

//std::shared_ptr<Session> SessionManager::createSession()
//{
//    // Закрываем сессию
//    //closeCurrentSession();
//    // Создаём сессию
//    mCurrentSession = std::make_shared<Session>(mDataBase);
//    // Установка времени начала сессии
//    mCurrentSession->mTimeStampStart = QDateTime::currentSecsSinceEpoch();

//    // Если нет БД
//    if (!mDataBase) {
//        mCurrentSession->mSessionID = LOCAL_SESSION_ID;
//        return mCurrentSession;
//    }
//    // Добавляем сессию
//    int id = 1; //mDataBase->insertSession(mCurrentSession.get()); // !!!!
//    // Если не удалось добавить сессию в БД
//    if (id == -1) {
//        mCurrentSession->mSessionID = LOCAL_SESSION_ID;
//    } else {
//        mCurrentSession->mSessionID = id;
//    }

//    return mCurrentSession;
//}

//bool SessionManager::closeCurrentSession()
//{
//    // Если текущей сессии не существует
//    if (!mCurrentSession) {
//        return false;
//    }

//    // Закрываем сессию
//    mCurrentSession->closeSession();

//    // Освобождаемся от текущей сессии
//    mCurrentSession.reset();

//    return true;
//}
