#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql>

/*! Таблица сессий */
namespace SessionTable {
    constexpr static char TableName[] = "Session";
    constexpr static char ID_Row[] = "session_id";
    constexpr static char TimeStampStart_Row[] = "time_stamp_start";
    constexpr static char TimeStampEnd_Row[] = "time_stamp_end";
    constexpr static char Info_Row[] = "info";
}

/*! Таблица меток */
namespace LabelTable {
    constexpr static char TableName[] = "Label";
    constexpr static char ID_Row[] = "label_id";
    constexpr static char TimeStampStart_Row[] = "time_stamp_start";
    constexpr static char TimeStampEnd_Row[] = "time_stamp_end";
    constexpr static char Number_Row[] = "number";
    constexpr static char Info_Row[] = "info";
    constexpr static char FK_Row[] = "session_id";
}

/*! Таблица автоинкрементирования */
namespace SequenceTable {
    constexpr static char TableName[] = "sqlite_sequence";
    constexpr static char Name_Row[] = "name";
    constexpr static char Sequence_Row[] = "seq";
}

class Session;
class Label;

/*! Менеджер базы данных
    1) Открывает/Закрывает соединение с БД
    2) Отправляет запросы БД
*/
class DataBaseManager
{
    /*! БД SQLite3 */
    QSqlDatabase *mDataBase {nullptr};

    /*! Для генерации всех запросов к БД */
    QSqlQuery *mQuery {nullptr};

    /*! Имя соединения с БД */
    const QString ConnectionName {"mainDB"};

public:
    DataBaseManager();

    /*! Прекращение работы */
    void terminate();

    /*! Открытие (создание) базы данных приложения
        path - путь к дирректории, где располагается файл БД
    */
    bool openDataBase(const QString &path);

    /*! Закрытие базы данных */
    void closeDataBase();

    /*! Возвращает информацию о файле БД, если она есть */
    QFileInfo dataBaseFile() const;

    /*! Открыто ли соединение с базой данных */
    bool isOpenConnection() const;
    /*################################################################################
                                    Общие функции
    ################################################################################*/
    /*! Возвращает последний id для таблиц
        с автоинкрементированным значением.
        Возвращает -1, если произошла ошибка запроса
        Возвращает 0, если результат запроса пустой
     */
    int selectLastID(const QString &tableName) const;
    /*################################################################################
                                        Сессии
    ################################################################################*/
    /*! Добавляет сессию в БД */
    //int insertSession(Session *session) const;

    /*! Обновление информации у сессии
        false - запрос не выполнен или произошла ошибка
    */
    bool updateSessionInfo(int sessionID, const QString &info) const;

    /*! Обновление времени окончания сессии */
    bool updateEndTimeSession(int sessionID, const int64_t &endSessionTime) const;

    /*! Выбирает все сессии */
    QVector<std::shared_ptr<Session>> selectAllSessions();
    /*################################################################################
                                         Метки
    ################################################################################*/
    /*! Добавляет метку в БД */
    //int insertLabel(Label *label) const;

    /*! Обновление информации у метки
        false - запрос не выполнен или произошла ошибка
    */
    //bool updateLabelInfo(int labelID, const QString &info) const;

    /*! Выбирает все метки, принаджежащие сессии */
    QVector<std::shared_ptr<Label>> selectAllLabelsBelongSession(int sessionID);

private:
    /*################################################################################
                                    Общие функции
    ################################################################################*/
    /*! Запрос к БД
        false - запрос не выполнен или произошла ошибка
    */
    bool query(const QString &query) const;

    /*! Запрос для вставки
        Возвращает -1, если произошла ошибка
        Возвращает вставленный id.
    */
    int insertQuery(const QString &query, const QString &tableName) const;

    /*! Запрос на обновление в таблице
        true - успешное обновление;
        false - ошибка.
    */
    bool updateQuery(const QString &query) const;

    /*! Обходит весь запрос выборки */
    template <class T>
    QVector<std::shared_ptr<T>> walkSelectQuery(std::shared_ptr<T>(*parseFunction)(QSqlQuery *query, DataBaseManager *dataBase));
    /*################################################################################
                                        Сессии
    ################################################################################*/
    /*! Парсит сессию из запроса */
    static std::shared_ptr<Session> createSessionFromRowQuery(QSqlQuery *query, DataBaseManager *dataBase);
    /*################################################################################
                                         Метки
    ################################################################################*/
    /*! Парсит метку из запроса */
    static std::shared_ptr<Label> createLabelFromRowQuery(QSqlQuery *query, DataBaseManager *dataBase);
};



#endif // DATABASEMANAGER_H
