#ifndef LABELMANAGER_H
#define LABELMANAGER_H

#include <memory>
#include <chrono>
#include <QVector>

class Label;
//class DataBaseManager;
class LabelProvider;
class Settings;

/*! Менеджер меток
    1) Создаёт метки по просьбе пользователя;
    2) Взаимодействует с базой данных для
    получения из неё информации о метках.
*/


class LabelManager
{
    /*! База данных */
    //DataBaseManager *mDataBase {nullptr};

    /*! Настройки приложения */
    Settings *mSettings {nullptr};

    /*! Последнее время создания метки */
    std::chrono::time_point<std::chrono::system_clock,
    std::chrono::duration<long long,
    std::ratio<1, 1000000000>>> mLastCreatedLabelTime {};

    /*! Время перезарядки метки в мс */
    const int mRechargeLabelTimer = 1000;

    /*! Последняя созданная метка */
    std::shared_ptr<Label> mLastCreatedLabel;

public:

    /*! Количество сделанных меток в текущей сессии (Зависит от mCurrentSessionID) */
    int mCountLabels = 0;

    explicit LabelManager(Settings *settings);//, DataBaseManager *dataBase);

    /*! Создаёт метку и возвращает её
        Если метку не удалось создать,
        возвращается пустой shared_ptr.
    */
    std::shared_ptr<Label> createLabel(const QString &infoText = QString(), int64_t endTimeLabel = -1);

    /*! Возвращает последнюю созданную метку */
    std::shared_ptr<Label> getLastCreatedLabel() const;

    /*! Возвращает кол-во сделанных меток за текущую сессию */
    int getLabelsCountPerCurrentSession() const;

    void deleteLabel();
};

#endif // LABELMANAGER_H
