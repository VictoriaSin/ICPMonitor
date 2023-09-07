#include "labelprovider.h"
#include "controller/labels/label.h"
#include "controller/labels/labelmanager.h"
#include "controller/settings.h"

#include <QDebug>

LabelProvider::LabelProvider(LabelManager *labelManager) :
    mParent(labelManager)
{

}

bool LabelProvider::getLabelsOnlyForward(int sessionID, int64_t sensorReadingTimeStampMs, int64_t labelSearchIntervalMS, QVector<Label *> *container)
{
    // Если пришёл nullptr или пришедшее значение меньше, чем предыдущее
    if (!container || mLastTimeStampSensorReadingMS > sensorReadingTimeStampMs) {
        return false;
    }

    // Сохраняем время показания датчика
    mLastTimeStampSensorReadingMS = sensorReadingTimeStampMs;

    // Если сессия сменилась
    if (changeSessionID(sessionID)) {
        // Подгружаем метки для другой сессии
        mLabelsBelongSessionID = takeLabelsForSession(sessionID);

        // Обнуляем итератор по меткам
        mLabelCurrentIt = 0;

        // Передвигаем итератор метки, если время метки оказалось меньше времени показания датчика
        movingLabelIteratorToSensorReadingTime(sensorReadingTimeStampMs, labelSearchIntervalMS);
    }

    // Добавляем в контейнер метки, относящиеся к показанию датчика
    while (0 <= mLabelCurrentIt && mLabelCurrentIt < mLabelsBelongSessionID.size()) {
        // Берём метку
        const auto &label = mLabelsBelongSessionID.at(mLabelCurrentIt);

        // Если метка NULL, передвигаем итератор меток
        if (!label) {
            ++mLabelCurrentIt;
            continue;
        }

        // Узнаём время метки
        auto timeStampLabelMS = label->getTimeStartLabelMS();

        // Если метка относится к пришедшему показанию датчика, добавляем в контейнер
        if ((sensorReadingTimeStampMs - labelSearchIntervalMS) <= timeStampLabelMS &&
                timeStampLabelMS <= sensorReadingTimeStampMs) {
            container->append(label.get());
        } else {
            break;
        }

        // Перемещаем итератор на следующую метку
        ++mLabelCurrentIt;
    }

    return true;
}

bool LabelProvider::changeSessionID(int sessionID)
{
    // Если и так стоит данная сессия
    if (mSessionID == sessionID) {
        return false;
    }

    // Меняем id сессии
    mSessionID = sessionID;

    return true;
}

QVector<std::shared_ptr<Label>> LabelProvider::takeLabelsForSession(int sessionID)
{
    // Если родитель не указан
    if (!mParent) {
        return {};
    }
    QVector<std::shared_ptr<Label>> lab;
    return lab;
    //return mParent->getAllLabelsBelongSession(sessionID);
}

void LabelProvider::movingLabelIteratorToSensorReadingTime(int64_t sensorReadingTime, int64_t labelSearchIntervalMS)
{
    while (mLabelCurrentIt < mLabelsBelongSessionID.size()) {
        // Берем метку
        const auto& label = mLabelsBelongSessionID.at(mLabelCurrentIt);

        // Если метка NULL, передвигаем итератор
        if (!label) {
            ++mLabelCurrentIt;
            continue;
        }

        // Если метка стала впереди на временном интервале
        // или равна времени показания датчика, заканчиваем передвижение
        if (label->getTimeStartLabelMS() - sensorReadingTime + labelSearchIntervalMS >= 0) {
            break;
        }

        // Передвигаем итератор
        ++mLabelCurrentIt;
    }
}

