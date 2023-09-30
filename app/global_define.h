#ifndef GLOBAL_DEFINE_H
#define GLOBAL_DEFINE_H
#include <QFile>
#include <QString>
#include <stdint.h>

#include <controller/settings.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t

#define BUFFER_1 1
#define BUFFER_2 2

#define getCurrentTimeStampMS() (QDateTime::currentMSecsSinceEpoch()/1000)
#define getCurrentTimeStamp_ms() QDateTime::currentMSecsSinceEpoch()
extern uint64_t startTimeStampRecord;
extern bool isLabelCreating;
extern bool isIntervalCreating;
extern uint32_t mCoordLabelX;
extern uint8_t mIntervalsCount;
extern uint16_t mCurrentLabelIndex;

extern QString mntDirectory;
extern Settings *mICPSettings;

extern QFile mRawDataFile;

#define MAXBUFFERSIZE 400
extern float intervalCountPerSec; // Интервал между соседними измерениями

#pragma pack(1)
typedef struct
{
    uint32_t timeStamp; // offset
    uint16_t data; //
}_mSPIData;

#pragma pack()

extern _mSPIData *mAllRecordedDataBuffer, *mFirstIntervalBuffer, *mSecondIntervalBuffer;//*mSensorBuffer1, *mSensorBuffer2;
extern uint mSizeAllRecordedData, mSizeFirstInterval, mSizeSecondInterval;

extern float mRecordedMaxXRange;
extern float mCurrentMaxYRange;
extern QPair<float, float> mFirstIntervalMinMaxXRange;
extern QPair<float, float> mSecondIntervalMinMaxXRange;

enum direction { previous = false, next = true};
enum interval { first = false, second = true};

#define TIME_INTERVAL_FOR_RECORD_IN_FILE (2)
#define TIME_INTERVAL_FOR_WRITE_ON_GRAPH (40) //40 миллисекунд - 25 раз в секунду
#define TIME_INTERVAL_DIFF (TIME_INTERVAL_FOR_WRITE_ON_GRAPH / TIME_INTERVAL_FOR_RECORD_IN_FILE)

#define READ_SPI_DATA() temp.data = (uint16_t)(30 + 15 * sin(3.14 * (double)((temp.timeStamp % 2000) + 1)/500));//(12 + rand() % 20)

#endif // GLOBAL_DEFINE_H
