#include "label.h"
#include <QDebug>

//uint32_t mCoordLabelX {0};

Label::Label()
{

}

bool Label::setLabelInfo(const QString &infoText)
{
    // Если БД не существует или ID не положительный или метка не привязана к сессии
    if (/*!mDB ||*/ mSessionID < 1 || mLabelID < 1)
    {
        return false;
    }

    // Если запрос не удачный
    //if (!mDB->updateLabelInfo(mLabelID, infoText)) { return false; } !!!!

    // Устанавливаем новую информацию
    mInfo = infoText;

    return true;
}

int Label::getId() const
{
    return mLabelID;
}

const int64_t &Label::getTimeStartLabelMS() const
{
    return mTimeStampStartMS;
}

const int64_t &Label::getTimeEndLabelMS() const
{
    return mTimeStampEndMS;
}

int Label::getNumberLabel() const
{
    return mNumber;
}

const QString &Label::getInfoLabel() const
{
    return mInfo;
}

int Label::getBelongIdSession() const
{
    return mSessionID;
}

QDebug operator<<(QDebug debug, const Label &val)
{
    debug << "Label(ID:" << val.getId() << ", StartTimeMS:" << val.getTimeStartLabelMS()
          << ", EndTimeMS:" << val.getTimeEndLabelMS() << ", Number:" << val.getNumberLabel()
          << ", Info:" << val.getInfoLabel() << ')' << ", SessionID:" << val.getBelongIdSession();

    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const std::shared_ptr<Label> &val) {
    if (!val) {
        return debug << "Null reference!";
    }
    return debug << *val.get();
}
