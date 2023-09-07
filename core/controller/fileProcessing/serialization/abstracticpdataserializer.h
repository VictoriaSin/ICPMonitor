#ifndef ABSTRACTICPDATASERIALIZER_H
#define ABSTRACTICPDATASERIALIZER_H

#include "idataserializer.h"

struct ComplexValue;

class AbstractICPDataSerializer : public IDataSerializer
{
public:
    AbstractICPDataSerializer();

    ComplexValue *sensorData {nullptr};
    bool alarmState = false;

    /*! Принадлежность показания к сессии */
    int sessionID {-1};

    // IDataSerializer interface
public:
    virtual ~AbstractICPDataSerializer() override = default;
    virtual void setAbsoluteTime(int64_t) override;
    virtual void reset() override;
    void setSesionID(int sessionID) override;
    int getSesionID() const override;
};


class AverageICPSerializer : public AbstractICPDataSerializer
{
public:
    AverageICPSerializer();

    // IDataSerializer interface
public:
    int serializeData(char *dest) const override;
    int deserializeData(const char *data) override;
    int64_t getAbsoluteTime() const final;
    int getMaxReadOrWriteData() const final;
    void setAbsoluteTime(int64_t absoluteTime) override;
    ~AverageICPSerializer() override {};
};


class OutputICPSerializer : public AbstractICPDataSerializer
{
public:
    OutputICPSerializer();

    // IDataSerializer interface
public:
    int serializeData(char *dest) const override;
    int deserializeData(char const *data) override;
    int64_t getAbsoluteTime() const final;
    int getMaxReadOrWriteData() const final;
    ~OutputICPSerializer() override {};
};

#endif // ABSTRACTICPDATASERIALIZER_H
