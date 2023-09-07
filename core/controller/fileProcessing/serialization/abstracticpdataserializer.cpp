#include "abstracticpdataserializer.h"
#include "global_structs.h"

#include <QDateTime>

AbstractICPDataSerializer::AbstractICPDataSerializer()
{

}

void AbstractICPDataSerializer::setAbsoluteTime(int64_t absoluteTime)
{
    if (!sensorData) {
        return;
    }

    sensorData->timestamp = absoluteTime;
}

void AbstractICPDataSerializer::reset()
{
    sensorData->clear();
    alarmState = false;
    sessionID = -1;
}

void AbstractICPDataSerializer::setSesionID(int sessionID)
{
    this->sessionID = sessionID;
}

int AbstractICPDataSerializer::getSesionID() const
{
    return sessionID;
}




AverageICPSerializer::AverageICPSerializer()
{

}

int AverageICPSerializer::serializeData(char *dest) const
{
    if (!sensorData) {
        return 0;
    }

    char *head = dest;

    *(int16_t *)head = sensorData->value * 100;
    head += 2;

    if (alarmState) {
        *head |= 0x02;
    }

    if (sensorData->valid) {
        *head |= 0x01;
    }

    return 3; // Пока что 3
}

int AverageICPSerializer::deserializeData(const char *data)
{
    if (!sensorData) {
        return 0;
    }

    const char *head = data;

    sensorData->value = *(int16_t *)head / 100.0;

    head += 2;

    sensorData->valid = *head & 0x01;
    alarmState = (*head & 0x02) == 2;

    return 3;
}

int64_t AverageICPSerializer::getAbsoluteTime() const
{
    if (!sensorData) {
        return 0;
    }

    return sensorData->timestamp / 1000;
}

int AverageICPSerializer::getMaxReadOrWriteData() const
{
    return 3; // Пока что 3
}

void AverageICPSerializer::setAbsoluteTime(int64_t absoluteTime)
{
    if (!sensorData) {
        return;
    }

    sensorData->timestamp = absoluteTime * 1000;
}




OutputICPSerializer::OutputICPSerializer()
{

}

int OutputICPSerializer::serializeData(char *dest) const
{
    if (!sensorData) {
        return 0;
    }

    char *head = dest;

    *(int16_t *)head = sensorData->value * 100;
    head += 2;

    if (alarmState) {
        *head |= 0x02;
    }

    if (sensorData->valid) {
        *head |= 0x01;
    }

    return 3;
}

int OutputICPSerializer::deserializeData(const char *data)
{
    if (!sensorData) {
        return 0;
    }

    const char *head = data;

    sensorData->value = *(int16_t *)head / 100.0;

    head += 2;

    sensorData->valid = *head & 0x01;
    alarmState = (*head & 0x02) == 2;

    return 3;
}

int64_t OutputICPSerializer::getAbsoluteTime() const
{
    if (!sensorData) {
        return 0;
    }

    return sensorData->timestamp;
}

int OutputICPSerializer::getMaxReadOrWriteData() const
{
    return 3;
}
