#include "dateconversion.h"
#include "ultrafastconverting/countlut.h"

#include <time.h>

/*! Для конвертации timestamp.
    В 2038 году произойдёт
    переполнение из-за функции localtime.
 */
struct tm *mTs {nullptr};
long mTimestamp = 0;
int mShift = 0;
int mAllShift = 0;

int convertStrfTime(int64_t timestampMs, char *buffer)
{
    mShift = 0;
    mTimestamp = timestampMs / 1000;
    mTs = localtime(&mTimestamp);

    mShift += strftime(buffer, 23, "%d-%m-%Y %H:%M:%S", mTs);
    buffer += mShift;

    *buffer = ':';
    ++mShift;
    ++buffer;
    mShift += i32toa_countlut(timestampMs % 1000, buffer);

    return mShift;
}

int convertDateCountLut(int64_t timestampMs, char *buffer)
{
    mAllShift = 0;
    mShift = 0;
    mTimestamp = timestampMs / 1000;
    mTs = localtime(&mTimestamp);

    // День
    mShift = u32toa_countlut(mTs->tm_mday, buffer);
    buffer += mShift;
    mAllShift += mShift;
    *buffer = '-';
    ++mAllShift;
    ++buffer;

    // Месяц
    mShift = u32toa_countlut(mTs->tm_mon + 1, buffer);
    buffer += mShift;
    mAllShift += mShift;
    *buffer = '-';
    ++mAllShift;
    ++buffer;

    // Год
    mShift = u32toa_countlut(1900 + mTs->tm_year, buffer);
    buffer += mShift;
    mAllShift += mShift;
    *buffer = ' ';
    ++mAllShift;
    ++buffer;

    // Час
    mShift = u32toa_countlut(mTs->tm_hour, buffer);
    buffer += mShift;
    mAllShift += mShift;
    *buffer = ':';
    ++mAllShift;
    ++buffer;

    // Минуты
    mShift = u32toa_countlut(mTs->tm_min, buffer);
    buffer += mShift;
    mAllShift += mShift;
    *buffer = ':';
    ++mAllShift;
    ++buffer;

    // Секунды
    mShift = u32toa_countlut(mTs->tm_sec, buffer);
    buffer += mShift;
    mAllShift += mShift;
    *buffer = ':';
    ++mAllShift;
    ++buffer;

    // Миллисекунды
    mAllShift += u32toa_countlut(timestampMs % 1000, buffer);

    return mAllShift;
}

int convertDateTrained(int64_t timestampMs, char *buffer)
{
    mTimestamp = timestampMs / 1000;
    mTs = localtime(&mTimestamp);

    // День
    *(int16_t *)buffer = *(int16_t *)datesave::MonthsDaysHoursMinsSecs[mTs->tm_mday];
    buffer += 2;
    *buffer = '-';
    ++buffer;

    // Месяц
    *(int16_t *)buffer = *(int16_t *)datesave::MonthsDaysHoursMinsSecs[mTs->tm_mon + 1];
    buffer += 2;
    *buffer = '-';
    ++buffer;

    // Год
    *(int32_t *)buffer = *(int32_t *)datesave::years[mTs->tm_year - 100];
    buffer += 4;
    *buffer = ' ';
    ++buffer;

    // Час
    *(int16_t *)buffer = *(int16_t *)datesave::MonthsDaysHoursMinsSecs[mTs->tm_hour];
    buffer += 2;
    *buffer = ':';
    ++buffer;

    // Минуты
    *(int16_t *)buffer = *(int16_t *)datesave::MonthsDaysHoursMinsSecs[mTs->tm_min];
    buffer += 2;
    *buffer = ':';
    ++buffer;

    // Секунды
    *(int16_t *)buffer = *(int16_t *)datesave::MonthsDaysHoursMinsSecs[mTs->tm_sec];
    buffer += 2;
    *buffer = ':';
    ++buffer;

    // Миллисекунды
    return 20 + u32toa_countlut(timestampMs % 1000, buffer);
}
