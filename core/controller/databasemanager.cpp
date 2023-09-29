#include "databasemanager.h"
//#include "controller/sessions/session.h"
#include "controller/labels/label.h"

#include <QFile>

DataBaseManager::DataBaseManager() {}
//void DataBaseManager::terminate()
//{
//    //closeDataBase();
//}
//bool DataBaseManager::openDataBase(const QString &path)
//{
////    // Если соединение с БД уже открыто
////    //if (isOpenConnection()) {
////        return true;
////    //}
//
////    // Файл базы данных
////    QFile fileDB(path + '/' + QCoreApplication::applicationName() + ".sqlite");
//
////    mDataBase = new QSqlDatabase;
////    *mDataBase = QSqlDatabase::addDatabase("QSQLITE", ConnectionName);
////    mDataBase->setDatabaseName(fileDB.fileName());
//
////    // Устанавливаем права для БД
////    if (fileDB.exists()) {
////        fileDB.setPermissions(QFileDevice::ReadOwner | QFileDevice::ReadUser |
////                              QFileDevice::ReadGroup | QFileDevice::ReadOther |
////                              QFileDevice::WriteOwner | QFileDevice::WriteUser |
////                              QFileDevice::WriteGroup | QFileDevice::WriteOther);
////    }
//
////    // Если БД не открылась
////    if (!mDataBase->open()) {
////        closeDataBase();
////        return false;
////    }
//
////    // Устанавливаем базу данных для запросов
////    mQuery = new QSqlQuery(*mDataBase);
//
////    // Отключаем кэширование
////    mQuery->setForwardOnly(true);
//
////    // Создаём таблицу Сессий, если не создана
////    if (!mQuery->exec(QString("CREATE TABLE IF NOT EXISTS %1 ("
////                      "%2 INTEGER PRIMARY KEY AUTOINCREMENT,"
////                      "%3 BIGINT NOT NULL,"
////                      "%4 BIGINT,"
////                      "%5 VARCHAR(255)"
////                      ");").arg(SessionTable::TableName).arg(SessionTable::ID_Row)
////                      .arg(SessionTable::TimeStampStart_Row).arg(SessionTable::TimeStampEnd_Row)
////                      .arg(SessionTable::Info_Row)))
////    {
////        qDebug() << mQuery->lastError();
////        return false;
////    }
////    // Создаём таблицу Меток, если не создана
////    if (!mQuery->exec(QString("CREATE TABLE IF NOT EXISTS %1 ("
////                      "%2 INTEGER PRIMARY KEY AUTOINCREMENT,"
////                      "%3 BIGINT NOT NULL,"
////                      "%4 BIGINT,"
////                      "%5 TINYINT NOT NULL,"
////                      "%6 VARCHAR(255),"
////                      "%7 INTEGER NOT NULL,"
////                      "FOREIGN KEY(%7) REFERENCES %8(%9)"
////                      ");").arg(LabelTable::TableName).arg(LabelTable::ID_Row)
////                      .arg(LabelTable::TimeStampStart_Row).arg(LabelTable::TimeStampEnd_Row)
////                      .arg(LabelTable::Number_Row).arg(LabelTable::Info_Row)
////                      .arg(LabelTable::FK_Row).arg(SessionTable::TableName).arg(SessionTable::ID_Row)))
////    {
////        qDebug() << mQuery->lastError();
////        return false;
////    }
//
////    return true;
//}
//void DataBaseManager::closeDataBase()
//{
//    return;
////    // Удаляем объект для запросов к БД
////    if (mQuery) {
////        delete mQuery;
////        mQuery = nullptr;
////    }

////    // Закрываем и удаляем объект взаимодействия с БД
////    if (mDataBase) {
////        if (mDataBase->isOpen()) {
////            mDataBase->close();
////        }
////        delete mDataBase;
////        mDataBase = nullptr;
////    }

////    // Удаляем соединение с БД
////    QSqlDatabase::removeDatabase(ConnectionName);
//}
//QFileInfo DataBaseManager::dataBaseFile() const
//{
//    return {};
//    //if (!mDataBase) { return {}; }
//    //return QFileInfo(mDataBase->databaseName());
//}
//bool DataBaseManager::isOpenConnection() const
//{
//    return true;
////    if (!mDataBase) {
////        return false;
////    }
//
////    return mDataBase->isOpen();
//}
/*
int DataBaseManager::insertSession(Session *session) const
{
    // Если соединение с БД закрыто
    if (!isOpenConnection()) {
        return -1;
    }
    return insertQuery(QString("INSERT INTO %1(%2) VALUES (%3)")
                       .arg(SessionTable::TableName)
                       .arg(SessionTable::TimeStampStart_Row)
                       .arg(session->mTimeStampStart), SessionTable::TableName);
}*/
//!!!bool DataBaseManager::updateSessionInfo(int sessionID, const QString &info) const
//!!!{
//!!!    return true;
//!!!    // Если соединение с БД закрыто
//!!!    if (!isOpenConnection()) {
//!!!        return false;
//!!!    }
//!!!
//!!!    return updateQuery(QString("UPDATE %1 SET %2 = '%3' WHERE %4 = %5")
//!!!                 .arg(SessionTable::TableName).arg(SessionTable::Info_Row).arg(info)
//!!!                 .arg(SessionTable::ID_Row).arg(sessionID));
//!!!}
//bool DataBaseManager::updateEndTimeSession(int sessionID, const int64_t &endSessionTime) const
//{
//    return true;
////    // Если соединение с БД закрыто
////    if (!isOpenConnection()) {
////        return false;
////    }
////    return updateQuery(QString("UPDATE %1 SET %2 = %3 WHERE %4 = %5")
////                 .arg(SessionTable::TableName).arg(SessionTable::TimeStampEnd_Row).arg(endSessionTime)
////                 .arg(SessionTable::ID_Row).arg(sessionID));
//}
//QVector<std::shared_ptr<Session>> DataBaseManager::selectAllSessions()
//{
//    return {};
//    //// Если соединение с БД закрыто
//    //if (!isOpenConnection()) {
//    //    return {};
//    //}
//
//    //// Если запрос не выполнен или произошла ошибка
//    //if (!query(QString("SELECT * FROM %1").arg(SessionTable::TableName))) {
//    //    return {};
//    //}
//
//    //return walkSelectQuery<Session>(createSessionFromRowQuery);
//}
/*
int DataBaseManager::insertLabel(Label *label) const
{

    // Если соединение с БД закрыто
    if (!isOpenConnection()) { return -1; }

    // Генерируем запрос
    QString query = QString("INSERT INTO %1(%2, %3, %4, %5, %6) VALUES ")
            .arg(LabelTable::TableName).arg(LabelTable::TimeStampStart_Row)
            .arg(LabelTable::TimeStampEnd_Row).arg(LabelTable::Number_Row)
            .arg(LabelTable::Info_Row).arg(LabelTable::FK_Row);

    // Добавление в запрос время начала метки
    auto& timeStampStart = label->mTimeStampStartMS;
    if (timeStampStart < 0) {
        return -1;
    }
    query.append(QString("(%1, ").arg(timeStampStart));

    // Добавление в запрос время окончания метки
    auto& timeStampEnd = label->mTimeStampEndMS;
    if (timeStampEnd != -1) {
        query.append(QString("%1, ").arg(timeStampEnd));
    } else {
        query.append("NULL, ");
    }

    // Добавление в запрос номера метки
    auto& number = label->mNumber;
    if (number < 0) {
        return -1;
    }
    query.append(QString("%1, ").arg(number));

    // Добавление в запрос информации о метке
    auto& info = label->mInfo;
    if (!info.isEmpty()) {
        query.append(QString("%1, ").arg(info));
    } else {
        query.append("NULL, ");
    }

    // Добавление в запрос id сессии-родителя
    auto& sessionID = label->mSessionID;
    if (sessionID < 0) {
        return -1;
    }
    query.append(QString("%1)").arg(sessionID));

    return insertQuery(query, LabelTable::TableName);
}
*/
/*
bool DataBaseManager::updateLabelInfo(int labelID, const QString &info) const
{
    // Если соединение с БД закрыто
    if (!isOpenConnection()) {
        return false;
    }

    return query(QString("UPDATE %1 SET %2 = '%3' WHERE %4 = %5")
                 .arg(LabelTable::TableName).arg(LabelTable::Info_Row).arg(info)
                 .arg(LabelTable::ID_Row).arg(labelID));
}
*/
//QVector<std::shared_ptr<Label>> DataBaseManager::selectAllLabelsBelongSession(int /*sessionID*/)
//{
//    return {};
////    // Если соединение с БД закрыто
////    if (!isOpenConnection()) {
////        return {};
////    }

////    // Если запрос не выполнен или произошла ошибка
////    //if (!query(QString("SELECT * FROM %1 WHERE %2 = %3").arg(LabelTable::TableName).arg(LabelTable::FK_Row).arg(sessionID))) { return {}; }

////    //return walkSelectQuery<Label>(createLabelFromRowQuery);
//}
//int DataBaseManager::selectLastID(const QString &tableName) const
//{
//    return 0;
////    // Если соединение с БД закрыто
////    if (!isOpenConnection()) {
////        return -1;
////    }

////    // Выполняем запрос
////    if (!query(QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg(SequenceTable::Sequence_Row).arg(SequenceTable::TableName).arg(SequenceTable::Name_Row).arg(tableName))) { return -1;     }

////    // Читаем получаемый ID
////    if (mQuery->first()) {
////        const QVariant value = mQuery->value(SequenceTable::Sequence_Row);

////        // Если id не пустой
////        if (!value.isNull()) {
////            return value.toInt();
////        }
////    }

////    // Освобождаем ресурсы, делаем запрос не активным
////    mQuery->finish();

////    return 0;
//}
//int DataBaseManager::insertQuery(const QString &query, const QString &tableName) const
//{
//    return 1;
////    // Если соединение с БД закрыто
////    if (!isOpenConnection()) {
////        return -1;
////    }

////    // Получаем последний автоинкрементированный id
////    const int lastID = selectLastID(tableName);
////    if (lastID == -1) {
////        return -1;
////    }

////    // Выполняем запрос
////    if (!this->query(query)) {
////        return -1;
////    }

////    // Освобождаем ресурсы, делаем запрос не активным
////    mQuery->finish();

////    // Узнаём id вставленной сущности
////    const int id = selectLastID(tableName);
////    if (id == lastID || id == 0) {
////        return -1;
////    }
////    return id;
//}
//bool DataBaseManager::updateQuery(const QString &query) const
//{
//    return true;
////    // Если соединение с БД закрыто
////    if (!isOpenConnection()) {
////        return false;
////    }

////    // Получаем результат запроса
////    bool res = this->query(query);

////    // Освобождаем ресурсы, делаем запрос не активным
////    mQuery->finish();

////    return res;
//}
//bool DataBaseManager::query(const QString &query) const
//{
//    return true;
////    // Если соединение с БД закрыто
////    if (!isOpenConnection()) {
////        return false;
////    }
////qDebug() << query;
////    // Если запрос пустой
////    if (query.isEmpty() || query.isNull()) {
////        return false;
////    }
////    // Выполняем запрос
////    if (!mQuery->exec(query)) {
////        qDebug() << mQuery->lastError();
////        return false;
////    }

////    return true;
//}
//std::shared_ptr<Session> DataBaseManager::createSessionFromRowQuery(QSqlQuery *query, DataBaseManager *dataBase)
//{
//    return {};
// //   // Если запрос или БД пустые
// //   if (!query || !dataBase) { return {}; }

// //   // Был ли удачно закончен exec
// //   if (!query->isActive()) { return {}; }

// //   // Сессия
// //   std::shared_ptr<Session> session = std::make_shared<Session>(dataBase);

// //   // Успешность преобразования
// //   bool ok = false;

// //   session->mSessionID = query->value(SessionTable::ID_Row).toInt(&ok); if (!ok) { return {}; }

// //   session->mTimeStampStart = query->value(SessionTable::TimeStampStart_Row).toLongLong(&ok); if (!ok) { return {}; }

// //   session->mTimeStampEnd = query->value(SessionTable::TimeStampEnd_Row).toLongLong(&ok); if (!ok) { session->mTimeStampEnd = -1; }

// //   session->mInfo = query->value(SessionTable::Info_Row).toString();

// //   return session;
//}
//std::shared_ptr<Label> DataBaseManager::createLabelFromRowQuery(QSqlQuery *query, DataBaseManager *dataBase)
//{
//    return {};
////    // Если запрос или БД пустые
////    if (!query || !dataBase) {
////        return {};
////    }

////    // Был ли удачно закончен exec
////    if (!query->isActive()) {
////        return {};
////    }

////    // Метка
////    std::shared_ptr<Label> label = std::make_shared<Label>(dataBase);

////    // Успешность преобразования
////    bool ok = false;

////    label->mLabelID = query->value(LabelTable::ID_Row).toInt(&ok);
////    if (!ok) {
////        return {};
////    }

////    label->mTimeStampStartMS = query->value(LabelTable::TimeStampStart_Row).toLongLong(&ok);
////    if (!ok) {
////        return {};
////    }

////    label->mTimeStampEndMS = query->value(LabelTable::TimeStampEnd_Row).toLongLong(&ok);
////    if (!ok) {
////        label->mTimeStampEndMS = -1;
////    }

////    label->mNumber = query->value(LabelTable::Number_Row).toInt(&ok);
////    if (!ok) {
////        return {};
////    }

////    label->mInfo = query->value(LabelTable::Info_Row).toString();

////    label->mSessionID = query->value(LabelTable::FK_Row).toInt(&ok);
////    if (!ok) {
////        label->mSessionID = -1;
////    }

////    return label;
//}
//template<class T> QVector<std::shared_ptr<T>> DataBaseManager::walkSelectQuery(std::shared_ptr<T>(*parseFunction)(QSqlQuery *query, DataBaseManager *dataBase))
//{
//    // Создаём контейнер
//    QVector<std::shared_ptr<T>> container;
//
//    // Проходимся по всему ответу запроса
//    while (mQuery->next()) {
//        // Парсим сессию
//        std::shared_ptr<T> label = parseFunction(mQuery, this);
//
//        // Если в запросе не нашлось валидной сессии
//        if (!label) {
//            continue;
//        }
//
//        // Добавляем в контейнер
//        container.append(label);
//    }
//
//    // Освобождаем ресурсы, делаем запрос неактивным
//    mQuery->finish();
//
//    return container;
//}
