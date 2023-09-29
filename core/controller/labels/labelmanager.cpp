#include "labelmanager.h"
#include "controller/labels/label.h"
//#include "controller/sessions/session.h"
#include "controller/databasemanager.h"
#include "controller/labels/labelprovider.h"
#include "controller/settings.h"
#include "../app/global_define.h"

#include <QDateTime>

#include <chrono>


LabelManager::LabelManager(Settings *settings)//, DataBaseManager *dataBase) : mDataBase(dataBase),
      :mSettings(settings)
{

}

void LabelManager::terminate()
{

}


//void LabelManager::setSessionID(int sessionID)
//{
//    if (mCurrentSessionID != sessionID) {
//        mCurrentSessionID = sessionID;
//        mCountLabels = 0;
//    }
//}

void LabelManager::deleteLabel()
{
    // добавить функционал на удаление из БД, пока заглушка
    mCountLabels --;
    this->getLastCreatedLabel()->~Label();
}

std::shared_ptr<Label> LabelManager::createLabel(const QString &/*infoText*/, int64_t /*endTimeLabel*/)
{
    // Если нет БД или она не открыта, или текущая сессия не положительная или локальная
//    if (!mDataBase || !mDataBase->isOpenConnection() || mCurrentSessionID < 1 || mCurrentSessionID == LOCAL_SESSION_ID) {
//        return {};
//    }

    // Если метка еще не перезарядилась
    auto currentTimePoint = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>
            (currentTimePoint - mLastCreatedLabelTime).count() < mRechargeLabelTimer) {
        return {};
    }

    // Создаём метку
    std::shared_ptr<Label> label = std::make_shared<Label>(mDataBase);

    // Установка времени начала метки
    label->mTimeStampStartMS = QDateTime::currentMSecsSinceEpoch();

    // Установка времени окончания метки
//    if (endTimeLabel != -1) {
//        label->mTimeStampEndMS = endTimeLabel;
//    }

    // Установка номера метки
    label->mNumber = ++mCountLabels;

    // Установка информации о метке
//    if (!infoText.isEmpty()) {
//        label->mInfo = infoText;
//    }

    // Установка принадлежности к сессии (её id)
    //label->mSessionID = mCurrentSessionID;

    // Добавляем метку в базу данных
//    int id = 1;//mDataBase->insertLabel(label.get()); // !!!!
//    if (id == -1) {return {};}
//    label->mLabelID = id;

    // Устанавливаем время создания последней метки
    mLastCreatedLabelTime = currentTimePoint;
    // Сохраняем метку
    mLastCreatedLabel = label;

    return mLastCreatedLabel;
}

std::shared_ptr<Label> LabelManager::getLastCreatedLabel() const
{
    return mLastCreatedLabel;
}

int LabelManager::getLabelsCountPerCurrentSession() const
{
    return mCountLabels;
}

std::shared_ptr<LabelProvider> LabelManager::getLabelProvider()
{
    return std::make_shared<LabelProvider>(this);
}

//QVector<std::shared_ptr<Label>> LabelManager::getAllLabelsBelongSession(int sessionID)
//{
//    // Если нет БД
//    if (!mDataBase) {
//        return {};
//    }
//
//    return mDataBase->selectAllLabelsBelongSession(sessionID);
//}
