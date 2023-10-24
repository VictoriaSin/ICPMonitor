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
#define REGS_CNT 32
    const char defRegs[] = "0x0100 0x2000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x0000 0x07FF 0x1000 0x2000 0x0000 0x1000 0x2000 0x0000 0x0000 0x1F00 0x0000 0x0000 0xFFFF 0x0000 0x0048 0x0015 0x76A9 0x9F0C 0x0124 0x8060 0x15C9 0xE2A2 0x0000 0x0001";
    QString regString;
    QVariant rValue = mSettings->value("regString", regString);
    bool needSave = false;
    if (rValue != "")
    {
      regString = rValue.toString();
    }
    else
    {
      regString = defRegs;
      mSettings->setValue("regString", regString);
      needSave = true;
    }
    QStringList regList = regString.split(' ');
    if (regList.count() != REGS_CNT)
    {
      qDebug("Incorrect params regs");
      exit(66);
    }
    for (uint8_t i = 0; i < REGS_CNT; i++)
    {
        mRegValues[i] = regList[i].toUInt(nullptr, 16);
        //qDebug() << mRegValues[i];
    }
    mFontScaleFactor = mSettings->value("mFontScaleFactor", mFontScaleFactor).toFloat();
    mSettings->endGroup();
    if (needSave) {mSettings->sync(); }
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
    QString regString;
    for (uint8_t i=0; i<32; i++)
    {
        regString += "0x" + QString::number(mRegValues[i], 16).rightJustified(4, '0').toUpper();
        if (i != 31)
        {
            regString += " ";
        }
    }
    mSettings->setValue("regString", regString);
    mSettings->endGroup();
    mSettings->sync();
}

void Settings::setFontScaleFactor(float fontScaleFactor)
{
    mFontScaleFactor = fontScaleFactor;
}

void Settings::setRegsValues(uint16_t* regs)
{
    for (uint i=0; i<32; i++)
    {
        mRegValues[i] = regs[i];
    }
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

    mCurrentReadingsGraphIntervalX = mSettings->value("mCurrentReadingsGraphIntervalX", mCurrentReadingsGraphIntervalX).toFloat();
    mCurrentReadingsGraphIntervalY = mSettings->value("mCurrentReadingsGraphIntervalY", mCurrentReadingsGraphIntervalY).toFloat();

    mTickCountX = mSettings->value("mTickCountX", mTickCountX).toFloat();
    mTickCountY = mSettings->value("mTickCountY", mTickCountY).toFloat();

    mPressureUnitsIndex = mSettings->value("mPressureUnitsIndex", mPressureUnitsIndex).toUInt();
    mAverageIntervalSec = mSettings->value("mAverageIntervalSec", mAverageIntervalSec).toFloat();
    mSettings->endGroup();
}

void Settings::writeCurrentSensorReadingsSettings()
{
    mSettings->beginGroup(mCurrentSensorReadingsGroup);
    mSettings->setValue("mRelativeCurrentSensorReadingsPath", mRelativeCurrentSensorReadingsPath);
    mSettings->setValue("mMaxTimeStorageCurrentSensorReadingsMs", qlonglong(mMaxTimeStorageCurrentSensorReadingsMs));
    mSettings->setValue("mCurrentReadingsGraphIntervalX", QString::number(mCurrentReadingsGraphIntervalX));
    mSettings->setValue("mCurrentReadingsGraphIntervalY", QString::number(mCurrentReadingsGraphIntervalY));
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
