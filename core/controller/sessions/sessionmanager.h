#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <memory>

#include <QVector>

class Session;
class DataBaseManager;

/*! Менеджер сессиий
    1) Создаёт и завершает сессии;
    2) Взаимодействует с базой данных
    для получения из неё информации.
*/
class SessionManager
{
    /*! Текущая сессия */
    //std::shared_ptr<Session> mCurrentSession;

    /*! База данных */
    //DataBaseManager *mDataBase {nullptr};

public:
    explicit SessionManager(DataBaseManager *dataBase);

    /*! Создаёт сессию и возвращает её
        Если сессию не удалось создать,
        возвращается пустой shared_ptr.
    */
    //std::shared_ptr<Session> createSession();

    /*! Окончание текущей сессии
        true - сессия успешно окончена;
        false - отсутствует текущая сессия
     */
    //bool closeCurrentSession();

    /*! Возврат текущей сессии */
    //const std::shared_ptr<Session> &getCurrentSession() const;

    /*! Активна ли сессия */
    //bool isActiveSession() const;

    /*! Возвращает все сессии */
    //QVector<std::shared_ptr<Session>> getAllSession();

    /*! Перед уничтоженим объекта */
    //void terminate();
};

#endif // SESSIONMANAGER_H
