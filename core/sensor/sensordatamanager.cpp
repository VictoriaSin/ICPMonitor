#include "sensordatamanager.h"
#include "impl/fileimitsensor.h"

#include <QVector>
#include <QTextStream>
#include <QDateTime>

SensorDataManager::SensorDataManager()
{

}

bool SensorDataManager::readDataFromSensor()
{
    QVector<QPair<float, float>> mSensorData;
    QVector<float> onlyYPos;
    // Открываем файл для чтения
    QFile file("/imit_data/ICP_data_60sec.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Вычитываем файл
    QTextStream sdata(&file);
    bool okPars = false;
    while (!sdata.atEnd())
    {
        const QString line = sdata.readLine();
        const float value = line.toFloat(&okPars);
        if (okPars)
        {
            //mSensorReadings.append(value);
            onlyYPos.append(value);
        }
    }
    file.close();
    for (int i=0; i<24000; i++)
    {
        mSensorData[i].first = QDateTime::currentMSecsSinceEpoch(); //заменить
        mSensorData[i].second =onlyYPos[i % onlyYPos.size()];
    }

}
