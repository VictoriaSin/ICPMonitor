#ifndef LABELPROVIDER_H
#define LABELPROVIDER_H

#include <cstdint>
#include <memory>
#include <limits>

#include <QVector>

class Label;
class LabelManager;
class Settings;

/*! Поставщик меток (Использовать как временный объект)
    Сопоставляет метки с показаниями датчика.
    Работает по логике forward iterator при запрашивании меток к показаниям.
*/
class LabelProvider
{
    /*! Менеджер меток */
    LabelManager *mParent {nullptr};

    /*! Текущая сессия, к которой загружены метки */
    int mSessionID {-1};

    /*! Время последнего показания датчика */
    int64_t mLastTimeStampSensorReadingMS = std::numeric_limits<int64_t>::min();

    /*! Метки относящиеся к mSessionID */
    QVector<std::shared_ptr<Label>> mLabelsBelongSessionID;

    /*! Для итераций по меткам */
    int mLabelCurrentIt = 0;

public:
    LabelProvider(LabelManager *labelManager);

    /*! Выдаёт метки, если они относятся к данному временному значению.
        Временные значения sensorReadingTimeStampMs необходимо давать только в возрастающем порядке.
        Если значение приходит меньше, чем приходило прошлое, метки прекращают выдаваться.
        sessionID - id сессии, к которой нужны метки;
        sensorReadingTimeStampMs - время показания датчика, к которому проверяется наличие метки;
        labelSearchIntervalMS - интервал поиска меток;
        container - контейнер, куда будут помещаться найденные метки.
        Возвращаемый результат:
        false - container пришёл nullptr или пришедшее время показания датчика меньше предыдущего пришедшего времени;
        true - поиск меток был совершён, найденные значения помещены в container.

        При работе зависит от значения labelSearchIntervalMS.
        Основываясь на нем может определить, относится ли метка к пришедшему времени показанию датчика или нет.
    */
    bool getLabelsOnlyForward(int sessionID, int64_t sensorReadingTimeStampMs, int64_t labelSearchIntervalMS, QVector<Label *> *container);

private:
    /*! Сменяет ID сессии
        false - id сесси остался прежний;
        true - id сессии изменился
    */
    bool changeSessionID(int sessionID);

    /*! Запрашивает метки у Менеджера меток, относящиеся
        к определенной сессии
    */
    QVector<std::shared_ptr<Label>> takeLabelsForSession(int sessionID);

    /*! Передвижение итератора по меткам до первой метки,
        следующей после времени показания датчика sensorReadingTime
    */
    void movingLabelIteratorToSensorReadingTime(int64_t sensorReadingTime, int64_t labelSearchIntervalMS);
};

#endif // LABELPROVIDER_H
