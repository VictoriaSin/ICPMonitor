#ifndef SETTINGS_H
#define SETTINGS_H
#include <memory>

#include <QObject>
#include <QSettings>
#include <QLocale>

#include "languagesetting.h"

class Settings : public QObject
{
    Q_OBJECT

public:
    /*!
       Конструктор, принимает следующие параметры:
       path - путь к файлу настроек приложения
     */
    Settings(const QString &path);
    ~Settings();

    /*! Регистрация файла перевода lang формате *.qm */
    bool registrateLangFile(QLocale::Language lang, const QString &langFilePath);

    /*! Чтение всех настроек */
    void readAllSetting();

    /*! Запись всех настроек */
    void writeAllSetting();

    void setAllPressureParam(float mCurrentReadingsGraphIntervalY, float mCurrentReadingsGraphIntervalYHigh, float mTickCountY,
                             float mHighLevelAlarm, float mLowLevelAlarm);
    /*################################################################################
                                      Общие настройки
    ################################################################################*/
    void readGeneralSettings();
    float getFontScaleFactor() const { return mFontScaleFactor; };
    QLocale::Language getCurrentAppLanguage() const { return mLanguageSettings->getCurrentLanguage(); };
    QString getSoftwareStorageUUID() const { return mSoftwareStorageUUID; };
    QString getFlashDeviceMountPart() const { return mFlashDeviceMountPart; };
    int64_t getLastSavedDateTimestampSec() const { return mLastSavedDateTimestampSec; }; ///< unix timestamp    
    uint16_t* getRegValues() { return mRegValues; };
    float getACoefficient() const { return mACoefficient; };
    float getBCoefficient() const { return mBCoefficient; };

    /*################################################################################
                                Настройки уровней тревоги
    ################################################################################*/
    void readAlarmSettings();
    int getLowLevelAlarm() const { return mLowLevelAlarm; };
    int getHighLevelAlarm() const { return mHighLevelAlarm; };
    bool getLowLevelStateAlarm() const { return mLowLevelStateAlarm; };
    bool getHighLevelStateAlarm() const { return mHighLevelStateAlarm; };
    /*################################################################################
                            Настройки текущих показаний датчика
    ################################################################################*/
    void readCurrentSensorReadingsSettings();
    QString getRelativeCurrentSensorReadingsPath() const { return mRelativeCurrentSensorReadingsPath; };
    int64_t getMaxTimeStorageCurrentSensorReadingsMs() const { return mMaxTimeStorageCurrentSensorReadingsMs; };
    float getCurrentReadingsGraphIntervalX() const { return mCurrentReadingsGraphIntervalX; };
    //QVector<QPair<float, float>> getCurrentReadingsGraphYRanges() const { return mCurrentReadingsGraphYRanges; };
    float getCurrentReadingsGraphIntervalYLow() const { return mCurrentReadingsGraphIntervalYLow; };
    float getCurrentReadingsGraphIntervalYHigh() const { return mCurrentReadingsGraphIntervalYHigh; };
    float getCurrentTickCountX() const { return mTickCountX; };
    float getCurrentTickCountY() const { return mTickCountY; };
    uint8_t getCurrentPressureIndex() const { return mPressureUnitsIndex; };
    float getCurrentAverageIntervalSec() const { return mAverageIntervalSec; };
    /*################################################################################
                            Настройки средних показаний датчика
    ################################################################################*/
    void readAverageSensorReadingsSettings();
    const QString &getRelativeAverageSensorReadingsPath() const { return mRelativeAverageSensorReadingsPath; };
    int64_t getMaxTimeStorageAverageSensorReadingsSec() const { return mMaxTimeStorageAverageSensorReadingsSec; };
    int getConstIntervalBetweenAverageSensorReadingsMs() const { return mConstIntervalBetweenAverageSensorReadingsMs; };

    /*! Минимальное корректное время */
    static const QDateTime MinDateTime;

    /*! Базовый путь монтирования USB */
    static const QString BasePathForMount;

private:
    /*################################################################################
                                      Общие настройки
    ################################################################################*/
    void writeGeneralSettings();
    void setFontScaleFactor(float fontScaleFactor);


public:
    void setRegsValues(uint16_t* regs);
    bool setAppLanguage(QLocale::Language language);
    void setSoftwareStorageUUID(const QString &blockDevUUID);
    void setACoefficient(float currACoefficient);
    void setBCoefficient(float currBCoefficient);
private:
    void setLastSavedDateTimestampSec(int64_t timestamp);
    /*################################################################################
                                Настройки уровней тревоги
    ################################################################################*/
    void writeAlarmSettings();
    void setLowLevelAlarm(int lowLevelAlarm);
    void setHighLevelAlarm(int highLevelAlarm);
    void setLowLevelStateAlarm(bool lowLevelStateAlarm);
    void setHighLevelStateAlarm(bool highLevelStateAlarm);
    /*################################################################################
                            Настройки текущих показаний датчика
    ################################################################################*/
    void writeCurrentSensorReadingsSettings();
    void setRelativeCurrentSensorReadingsPath(const QString &relativeCurrentSensorReadingsPath);
    void setMaxTimeStorageCurrentSensorReadingsMs(int64_t maxMs);
//    void setCurrentReadingsGraphInterval(float currentReadingsGraphInterval);
    /*################################################################################
                            Настройки средних показаний датчика
    ################################################################################*/
    void writeAverageSensorReadingsSettings();
    void setRelativeAverageSensorReadingsPath(const QString &relativeAverageSensorReadingsPath);
    void setMaxTimeStorageAverageSensorReadingsSec(int64_t maxSec);

private slots:
    void setCurrentReadingsGraphIntervalX(float currentReadingsGraphIntervalX);
    void setCurrentReadingsGraphIntervalY(float currentReadingsGraphIntervalYFrom, float currentReadingsGraphIntervalYTo);
    void setCurrentTickCountX(float currentTickCountX);
    void setCurrentTickCountY(float currentTickCountY);
    void setCurrentPressureUnits(uint8_t currentPressureUnitsIndex);
    void setCurrentAverageIntervalSec(float currentAverageIntervalSec);

private:
    QSettings *mSettings {nullptr};
    /*################################################################################
                                      Общие настройки
    ################################################################################*/
    const QString mGeneralGroup {"GeneralGroup"};
    std::unique_ptr<LanguageSetting> mLanguageSettings;
    QString mSoftwareStorageUUID;
    QString mFlashDeviceMountPart;
    int64_t mLastSavedDateTimestampSec {0};
    float mAverageIntervalSec;
    float mACoefficient {0};
    float mBCoefficient {0};

public:

    uint16_t mRegValues[32];

#ifdef PC_BUILD
    float mFontScaleFactor {1};
#else
    float mFontScaleFactor {3.6856};
#endif




    /*################################################################################
                                    Настройки скриншотов
    ################################################################################*/
    const QString mScreensGroup {"ScreensGroup"};
    /*################################################################################
                                Настройки уровней тревоги
    ################################################################################*/
    const QString mAlarmGroup {"AlarmGroup"};
    int mLowLevelAlarm {0};
    int mHighLevelAlarm {30};
    bool mLowLevelStateAlarm {false};
    bool mHighLevelStateAlarm {false};
    /*################################################################################
                            Настройки текущих показаний датчика
    ################################################################################*/
    const QString mCurrentSensorReadingsGroup {"CurrentSensorReadingsGroup"};
    QString mRelativeCurrentSensorReadingsPath {"./ICPCurrentSensorReadings"};
    int64_t mMaxTimeStorageCurrentSensorReadingsMs {1 * 24 * 60 * 60 * 1000};
    float mCurrentReadingsGraphIntervalX {12};
    float mCurrentReadingsGraphIntervalYLow {10};
    float mCurrentReadingsGraphIntervalYHigh {60};
    float mTickCountX {6};
    float mTickCountY {3};
    uint8_t mPressureUnitsIndex {0};

    /*QVector<QPair<float, float>> mCurrentReadingsGraphYRanges {
        {0, 30},    {0, 50},
        {0, 75},    {0, 100},
        {-10, 20},  {-50, 50}
    };*/
    /*################################################################################
                            Настройки средних показаний датчика
    ################################################################################*/
    const QString mAverageSensorReadingsGroup {"AverageSensorReadingsGroup"};
    QString mRelativeAverageSensorReadingsPath {"./ICPAverageSensorReadings"};
    int64_t mMaxTimeStorageAverageSensorReadingsSec {14 * 24 * 60 * 60};
    const int mConstIntervalBetweenAverageSensorReadingsMs {1000};

    friend class MonitorController;
};
#endif // SETTINGS_H
