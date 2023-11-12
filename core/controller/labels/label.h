#ifndef LABEL_H
#define LABEL_H

#include <memory>

#include <QString>
#include <qpair.h>




class LabelManager;
//class DataBaseManager;

/*! Метки
    К меткам имеют полный доступ
    Менеджер меток и Менеджер базы данных.
    Остальные могут изменять лишь информацию
    о сессии.
*/
class Label
{
    /*! Информация хранящаяся в БД */
    int mLabelID {-1};
    int64_t mTimeStampEndMS {-1};
    int mNumber {-1};
    QString mInfo;
    int mSessionID {-1};


    /*! Доступ к базе данных */
    //DataBaseManager* mDB {nullptr};

public:
    float mCurrentPos {0};
    int64_t mTimeStampStartMS {-1};
    explicit Label();//DataBaseManager *dataBase);

    /*! Установка информации о метке */
    bool setLabelInfo(const QString &infoText);

    /*! Возвращает id сессии */
    int getId() const;

    /*! Возвращает время начала метки */
    const int64_t &getTimeStartLabelMS() const;

    /*! Возвращает время окончания метки */
    const int64_t &getTimeEndLabelMS() const;

    /*! Возвращает номер метки */
    int getNumberLabel() const;

    /*! Возвращает информацию о метке */
    const QString &getInfoLabel() const;

    /*! Возвращает ID сессии, к которой относится */
    int getBelongIdSession() const;

    friend class LabelManager;
    friend class DataBaseManager;
};

QDebug operator<<(QDebug debug, const Label &val);
QDebug operator<<(QDebug debug, const std::shared_ptr<Label> &val);
#endif // LABEL_H
