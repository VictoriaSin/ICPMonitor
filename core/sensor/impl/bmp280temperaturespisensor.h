#ifndef BMP280TEMPERATURESPISENSOR_H
#define BMP280TEMPERATURESPISENSOR_H

#define BMP280_REG_TEMP_XLSB	0xFC
#define BMP280_REG_TEMP_LSB		0xFB
#define BMP280_REG_TEMP_MSB		0xFA

#define BMP280_REG_CONFIG		0xF5
#define BMP280_REG_CTRL_MEAS	0xF4

#define BMP280_REG_COMP_TEMP_1	0x88
#define BMP280_REG_COMP_TEMP_2	0x89
#define BMP280_REG_COMP_TEMP_3	0x8A
#define BMP280_REG_COMP_TEMP_4	0x8B
#define BMP280_REG_COMP_TEMP_5	0x8C
#define BMP280_REG_COMP_TEMP_6	0x8D

#include <QObject>
#include <QTimer>

#include "sensor/isensor.h"

class SPI;

/*! Датчик температуры, работающий по интерфейсу SPI
*/
class BMP280TemperatureSpiSensor : public ISensor
{
    Q_OBJECT

    /*! Карта всех свойств */
    QVariantMap mPropertyMap;

    /*! Таймер для запроса показаний */
    QTimer *mDataRequestTimer {nullptr};
    /*! Кол-во показаний датчика в секунду. От 1 до 1000 показаний. */
    int mReadPerSec {100};

    /*! Канал общения с датчиком */
    SPI *mSPIChannel {nullptr};

    /*! Регистры запроса температуры */
    const uint8_t mTempRequest[3] {BMP280_REG_TEMP_MSB,
                                   BMP280_REG_TEMP_LSB,
                                   BMP280_REG_TEMP_XLSB};

    /*! Ответ по запросу температуры */
    uint8_t mTempResponse[4] {};

    /*! Компенсационные параметры датчика */
    int32_t mTempCompensationParam[3] {};

    /*! Время последнего сброса датчика */
    int64_t mLastResetTimestamp {0};

    /*! Последние запрошенные данные с датчика */
    ComplexValue mLastData;

    /*! Текущий статус датчика */
    SensorStatus mStatus {SensorStatus::WaitInited};

    /*! Текущее кол-во подряд идущих ошибок инициализации SPI */
    int mCountInitError {0};
    /*! Максимально допустимое кол-во подряд идущих ошибок инициализации SPI */
    const int MaxCountInitError {5};
    /*! Кол-во миллисекунд, через которые запрашивается
        попытка повторной инициализации
    */
    const int mReInitMs {1000};

    /*! Текущее кол-во подряд идущих ошибок при запросе инфы с датчика */
    int mCountRequestError {0};
    /*! Максимально допустимое кол-во подряд идущих ошибок при запросе инфы с датчика */
    const int MaxCountRequestError {10};

    /*! Подключён ли датчик */
    bool mSensorConnected {false};

    #ifdef QT_DEBUG
        uint64_t benchTime = 0;
        uint64_t avgBenchTime = 0;
        int benchCount = 0;
    #endif

public:
    /*! Свойство (ключ) - путь к устройству SPI.
        Ожидает значение (Например: "/dev/spidev0.0")
    */
    static const QString PathToSPIDeviceProperty;

    /*! Свойство (ключ) - скорость общения с устройством Hz.
        Ожидает значение (Например: 100000)
    */
    static const QString SPISpeedProperty;

public:
    BMP280TemperatureSpiSensor();
    ~BMP280TemperatureSpiSensor() override;

    // ISensor interface
public:
    /*! Инициализация протокола SPI */
    bool init(const QVariantMap &args) override;
    bool isInit() const override;
    bool config(const QString &jsonFilePath) override;
    bool isConfig() const override;
    bool startSendingSensorReadings() override;
    bool isSensorConnected() const override;
    void endSendingSensorReadings() override;
    bool needMakeSensorReset() const override;
    bool canMakeSensorReset() const override;
    bool makeSensorReset() override;
    int64_t lastResetTimestamp() const override;
    ComplexValue readData() const override;
    SensorStatus status() const override;

    bool isAllPropertiesToInitAreSet() const override;
    bool hasSensorProperty(const QString &key) const override;
    QVariant sensorProperty(const QString &key) const override;
    QVariantMap sensorProperties() const override;
    bool setSensorProperty(const QString &key, const QVariant &value) override;
    QString info() const override;

protected slots:
    void checkSensorConnection() override;

private:
    /*! Преобразование считанного значения в цельсии */
    inline int32_t bmp280_compensate_T_int32(int32_t adc_T);

    /*! Была ли ошибка запроса датчика */
    bool hasErrorCurrentRequest();

    /*! Конфигурация BMP280 */
    bool configBMP280();

    /*! Чтение температурных компенсационных параметров BMP280
        false - не удачное чтение;
        true - удачное чтение.
    */
    bool takeTemperatureCompensationParametersBMP280();

private slots:
    /*! Запрос данных с датчика */
    void dataRequest() override;

    // ISensor interface
public:
    bool isStartedSendingSensorReadings() const override;
    void hardReset() override;
    /*! Используется при подключении/отключении датчика */
    void softReset() override;
    void setSensorProperties(const QVariantMap &properties) override;
    bool propertyHasBeenSet(const QString &key) const override;
    void resetPropertyMap() override;

};

#endif // BMP280TEMPERATURESPISENSOR_H
