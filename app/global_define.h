#ifndef GLOBAL_DEFINE_H
#define GLOBAL_DEFINE_H
#include <QString>
#include <stdint.h>

#include <controller/settings.h>

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t

#define getCurrentTimeStampMS() (QDateTime::currentMSecsSinceEpoch()/1000)
extern uint64_t startTimeStampRecord;
extern bool isLabelCreating;
extern bool isIntervalCreating;
extern uint32_t mCoordLabelX;
extern uint8_t mIntervalsCount;
extern uint16_t mCurrentLabelIndex;

extern QString mntDirectory;
extern Settings *mICPSettings;

#define maxBufferSize 400
extern struct mSPIBuffer
{
    uint16_t index;
    uint64_t data[maxBufferSize];
} mSensorBuffer1, mSensorBuffer2;

enum direction { previous = false, next = true};
enum interval { first = false, second = true};
#endif // GLOBAL_DEFINE_H
