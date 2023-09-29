#include "settings.h"

#include <QDebug>
#include <QDateTime>

const QDateTime Settings::MinDateTime = QDateTime(QDate(2000, 1, 1), QTime(0, 0));
const QString Settings::BasePathForMount = "/mnt";

Settings::Settings(const QString &path)
     : QObject()
     , mSettings(new QSettings(path, QSettings::IniFormat)),
       mLanguageSettings(std::make_unique<LanguageSetting>())
{
    qDebug() << "path=" << path;
}

Settings::~Settings()
{
    writeAllSetting();
}

bool Settings::registrateLangFile(QLocale::Language lang, const QString &langFilePath)
{
    return mLanguageSettings->registrateLangFile(lang, langFilePath);
}

void Settings::readAllSetting()
{
    readGeneralSettings();
    readAlarmSettings();
    readCurrentSensorReadingsSettings();
    readAverageSensorReadingsSettings();
    readScreensSettings();
}

void Settings::writeAllSetting()
{
    writeGeneralSettings();
    writeAlarmSettings();
    writeCurrentSensorReadingsSettings();
    writeAverageSensorReadingsSettings();
    writeScreensSettings();
}

void Settings::readGeneralSettings()
{
    mSettings->beginGroup(mGeneralGroup);
    mLanguageSettings->setAppLanguage(QLocale::Language(mSettings->value("mCurrentLanguage", mLanguageSettings->getCurrentLanguage()).toUInt()));
    mSoftwareStorageUUID = mSettings->value("mSoftwareStorageUUID", mSoftwareStorageUUID).toString();
    mFlashDeviceMountPart = mSettings->value("mFlashDeviceMountPart", mFlashDeviceMountPart).toString();
    //mLastSavedDateTimestampSec = mSettings->value("mLastSavedDateTimestampSec", qlonglong(mLastSavedDateTimestampSec)).toLongLong();
    mFontScaleFactor = mSettings->value("mFontScaleFactor", mFontScaleFactor).toFloat();
    mSettings->endGroup();
}

void Settings::writeGeneralSettings()
{
    mSettings->beginGroup(mGeneralGroup);
    mSettings->setValue("mCurrentLanguage", QString::number(mLanguageSettings->getCurrentLanguage()));//mLanguageSettings->getCurrentLanguage());
qDebug() << "lang" << QString::number(mLanguageSettings->getCurrentLanguage());
    mSettings->setValue("mSoftwareStorageUUID", mSoftwareStorageUUID);
    mSettings->setValue("mFlashDeviceMountPart", mFlashDeviceMountPart);
    //mSettings->setValue("mLastSavedDateTimestampSec", qlonglong(mLastSavedDateTimestampSec));
    mSettings->setValue("mFontScaleFactor", QString::number(mFontScaleFactor));
    mSettings->endGroup();
    mSettings->sync();
}

void Settings::setFontScaleFactor(float fontScaleFactor)
{
    mFontScaleFactor = fontScaleFactor;
}

bool Settings::setAppLanguage(QLocale::Language language)
{
    return mLanguageSettings->setAppLanguage(language);
}

void Settings::setSoftwareStorageUUID(const QString &blockDevUUID)
{
    mSoftwareStorageUUID = blockDevUUID;
}

void Settings::setLastSavedDateTimestampSec(int64_t /*timestamp*/)
{
    //mLastSavedDateTimestampSec = timestamp;
}

void Settings::readScreensSettings()
{
    mSettings->beginGroup(mScreensGroup);
    mMaxScreens = mSettings->value("mMaxScreens", mMaxScreens).toUInt();
    mRelativeScreensPath = mSettings->value("mRelativeScreensPath", mRelativeScreensPath).toString();
    mSettings->endGroup();
}

void Settings::writeScreensSettings()
{
    mSettings->beginGroup(mScreensGroup);
    mSettings->setValue("mMaxScreens", mMaxScreens);
    mSettings->setValue("mRelativeScreensPath", mRelativeScreensPath);
    mSettings->endGroup();
    mSettings->sync();
}

void Settings::setMaxScreens(uint maxScreens)
{
    mMaxScreens = maxScreens;
}

void Settings::setRelativeScreensPath(const QString &relativeScreensPath)
{
    mRelativeScreensPath = relativeScreensPath;
}

void Settings::readAlarmSettings()
{
    mSettings->beginGroup(mAlarmGroup);
    mLowLevelAlarm = mSettings->value("mLowLevelAlarm", mLowLevelAlarm).toInt();
    mHighLevelAlarm = mSettings->value("mHighLevelAlarm", mHighLevelAlarm).toInt();
    mLowLevelStateAlarm = mSettings->value("mLowLevelStateAlarm", mLowLevelStateAlarm).toBool();
    mHighLevelStateAlarm = mSettings->value("mHighLevelStateAlarm", mHighLevelStateAlarm).toBool();
    mSettings->endGroup();
}

void Settings::writeAlarmSettings()
{
    mSettings->beginGroup(mAlarmGroup);
    mSettings->setValue("mLowLevelAlarm", mLowLevelAlarm);
    mSettings->setValue("mHighLevelAlarm", mHighLevelAlarm);
    mSettings->setValue("mLowLevelStateAlarm", mLowLevelStateAlarm);
    mSettings->setValue("mHighLevelStateAlarm", mHighLevelStateAlarm);
    mSettings->endGroup();
    mSettings->sync();
}

void Settings::setLowLevelAlarm(int lowLevelAlarm)
{
    mLowLevelAlarm = lowLevelAlarm;
}

void Settings::setHighLevelAlarm(int highLevelAlarm)
{
    mHighLevelAlarm = highLevelAlarm;
}

void Settings::setLowLevelStateAlarm(bool lowLevelStateAlarm)
{
    mLowLevelStateAlarm = lowLevelStateAlarm;
}

void Settings::setHighLevelStateAlarm(bool highLevelStateAlarm)
{
    mHighLevelStateAlarm = highLevelStateAlarm;
}

void Settings::readCurrentSensorReadingsSettings()
{
    mSettings->beginGroup(mCurrentSensorReadingsGroup);
    mRelativeCurrentSensorReadingsPath = mSettings->value("mRelativeCurrentSensorReadingsPath", mRelativeCurrentSensorReadingsPath).toString();
    mMaxTimeStorageCurrentSensorReadingsMs = mSettings->value("mMaxTimeStorageCurrentSensorReadingsMs", qlonglong(mMaxTimeStorageCurrentSensorReadingsMs)).toLongLong();

    mCurrentReadingsGraphIntervalX = mSettings->value("mCurrentReadingsGraphIntervalX", mCurrentReadingsGraphIntervalX).toDouble();    
    mCurrentReadingsGraphIntervalY = mSettings->value("mCurrentReadingsGraphIntervalY", mCurrentReadingsGraphIntervalY).toDouble();

    mTickCountX = mSettings->value("mTickCountX", mTickCountX).toDouble();
    mTickCountY = mSettings->value("mTickCountY", mTickCountY).toDouble();

    mPressureUnitsIndex = mSettings->value("mPressureUnitsIndex", mPressureUnitsIndex).toUInt();
    mAverageIntervalSec = mSettings->value("mAverageIntervalSec", mAverageIntervalSec).toFloat();
    mSettings->endGroup();
}

void Settings::writeCurrentSensorReadingsSettings()
{
    mSettings->beginGroup(mCurrentSensorReadingsGroup);
    mSettings->setValue("mRelativeCurrentSensorReadingsPath", mRelativeCurrentSensorReadingsPath);
    mSettings->setValue("mMaxTimeStorageCurrentSensorReadingsMs", qlonglong(mMaxTimeStorageCurrentSensorReadingsMs));
    mSettings->setValue("mCurrentReadingsGraphIntervalX", mCurrentReadingsGraphIntervalX);
    mSettings->setValue("mCurrentReadingsGraphIntervalY", mCurrentReadingsGraphIntervalY);
    mSettings->setValue("mTickCountX", QString::number(mTickCountX));
    mSettings->setValue("mTickCountY", QString::number(mTickCountY));
    mSettings->setValue("mPressureUnitsIndex", mPressureUnitsIndex);
    mSettings->setValue("mAverageIntervalSec", QString::number(mAverageIntervalSec));
    mSettings->endGroup();
    mSettings->sync();
}

void Settings::setRelativeCurrentSensorReadingsPath(const QString &relativeCurrentSensorReadingsPath)
{
    mRelativeCurrentSensorReadingsPath = relativeCurrentSensorReadingsPath;
}

void Settings::setMaxTimeStorageCurrentSensorReadingsMs(int64_t maxMs)
{
    mMaxTimeStorageCurrentSensorReadingsMs = maxMs;
}

void Settings::setCurrentReadingsGraphIntervalX(float currentReadingsGraphIntervalX)
{
    mCurrentReadingsGraphIntervalX = currentReadingsGraphIntervalX;
}

void Settings::setCurrentReadingsGraphIntervalY(float currentReadingsGraphIntervalY)
{
    mCurrentReadingsGraphIntervalY = currentReadingsGraphIntervalY;
}

void Settings::setCurrentTickCountX(float currentTickCountX)
{
    mTickCountX = currentTickCountX;
}

void Settings::setCurrentTickCountY(float currentTickCountY)
{
    mTickCountY = currentTickCountY;
}

void Settings::setCurrentPressureUnits(uint8_t currentPressureUnitsIndex)
{
    mPressureUnitsIndex = currentPressureUnitsIndex;
}

void Settings::setCurrentAverageIntervalSec(float currentAverageIntervalSec)
{
    mAverageIntervalSec = currentAverageIntervalSec;
}

void Settings::setAllPressureParam(float mCurrentReadingsGraphIntervalY, float mTickCountY,
                         float mHighLevelAlarm, float mLowLevelAlarm)
{
    setCurrentReadingsGraphIntervalY(mCurrentReadingsGraphIntervalY);
    setCurrentTickCountY(mTickCountY);
    setHighLevelAlarm(mHighLevelAlarm);
    setLowLevelAlarm(mLowLevelAlarm);
    writeAlarmSettings();
    writeCurrentSensorReadingsSettings();
}


void Settings::readAverageSensorReadingsSettings()
{
    mSettings->beginGroup(mAverageSensorReadingsGroup);
    mRelativeAverageSensorReadingsPath = mSettings->value("mRelativeAverageSensorReadingsPath", mRelativeAverageSensorReadingsPath).toString();
    mMaxTimeStorageAverageSensorReadingsSec = mSettings->value("mMaxTimeStorageAverageSensorReadingsSec", qlonglong(mMaxTimeStorageAverageSensorReadingsSec)).toLongLong();
    mSettings->endGroup();
}

void Settings::writeAverageSensorReadingsSettings()
{
    mSettings->beginGroup(mAverageSensorReadingsGroup);
    mSettings->setValue("mRelativeAverageSensorReadingsPath", mRelativeAverageSensorReadingsPath);
    mSettings->setValue("mMaxTimeStorageAverageSensorReadingsSec", qlonglong(mMaxTimeStorageAverageSensorReadingsSec));
    mSettings->endGroup();
    mSettings->sync();
}

void Settings::setRelativeAverageSensorReadingsPath(const QString &relativeAverageSensorReadingsPath)
{
    mRelativeAverageSensorReadingsPath = relativeAverageSensorReadingsPath;
}

void Settings::setMaxTimeStorageAverageSensorReadingsSec(int64_t maxSec)
{
    mMaxTimeStorageAverageSensorReadingsSec = maxSec;
}
