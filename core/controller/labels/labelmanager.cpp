#include "labelmanager.h"
#include "controller/labels/label.h"
//#include "controller/sessions/session.h"
#include "controller/databasemanager.h"
#include "controller/labels/labelprovider.h"
#include "controller/settings.h"
#include "../app/global_define.h"

#include <QDateTime>

#include <chrono>


LabelManager::LabelManager(Settings *settings) : mSettings(settings)
{
}

void LabelManager::deleteLabel()
{
    // добавить функционал на удаление из БД, пока заглушка
    mCountLabels --;
    this->getLastCreatedLabel()->~Label();
}

std::shared_ptr<Label> LabelManager::createLabel(const QString &/*infoText*/, int64_t /*endTimeLabel*/)
{
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
    // Установка номера метки
    label->mNumber = ++mCountLabels;
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
