#ifndef SENSORDATAMANAGER_H
#define SENSORDATAMANAGER_H

#include <QObject>

class SensorDataManager
{
    Q_OBJECT
public:
    SensorDataManager();
protected:
    bool readDataFromSensor();
};

#endif // SENSORDATAMANAGER_H
