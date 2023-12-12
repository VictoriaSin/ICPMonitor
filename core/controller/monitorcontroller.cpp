#include "monitorcontroller.h"
#include "controller/settings.h"
#include "controller/averageicpcontroller.h"
#include "controller/alarmcontroller.h"
#include "controller/labels/labelmanager.h"
#include "controller/labels/label.h"
#include "blockDevices/blockdevicemanager.h"
#include "blockDevices/blockdevice.h"
#include "global_functions.h"
#include "../app/global_define.h"
#include <QDebug>
#include <QProcess>
#include <QDir>
#include "../app/clock.h"

LabelManager *mLabelManagerGlobal;
bool isLabelCreating {false};

MonitorController::MonitorController()
{
  mAverageICPController = new AverageICPController(mICPSettings, this);
  mAlarmController      = new AlarmController(mICPSettings, mAverageICPController, this);
  mLabelManagerGlobal   = new LabelManager(mICPSettings);
  connect(mAlarmController,  &AlarmController::alarmEvent, this,  &MonitorController::processAlarmEvent);
}
void MonitorController::terminate()
{
  DESTROY_CLASS(mAverageICPController);
  DESTROY_CLASS(mAlarmController);
  DESTROY_CLASS(mLabelManagerGlobal);
  DESTROY_CLASS(mICPSettings);
  emit destroyThread();
}
ControllerEvent MonitorController::getSoftwareStorageState() const
{
  return mSoftwareStorageState;
}
void MonitorController::setSoftwareStorage(const std::shared_ptr<BlockDevice> &devStorage)
{
  const QString devUUID = devStorage->getUUID();
  qDebug() << "New Software Storage";
  mICPSettings->setSoftwareStorageUUID(devUUID);
  mICPSettings->writeGeneralSettings();
}
std::shared_ptr<BlockDevice> MonitorController::getSoftwareStorage() const
{
  return mSoftwareStorage;
}
//QList<std::shared_ptr<BlockDevice> > MonitorController::getAvailableBlockDevices() const
//{
//  return {};
//}

Settings *MonitorController::settings() const
{
  return mICPSettings;
}
bool MonitorController::setAppLanguage(QLocale::Language language)
{
  if (mICPSettings->setAppLanguage(language)) { mICPSettings->writeAllSetting(); return true; }
  return false;
}
void MonitorController::makeLabel()
{
//  isLabelCreating = true;
//  // Если метка создалась, то рассылаем уведомление о создании с номером метки и временем создания
//  if (std::shared_ptr<Label> label = mLabelManagerGlobal->createLabel())
//  {
//    emit controllerEvent(ControllerEvent::LabelCreated, QVariantMap({
//                                                                      {ControllerEventsArgs::LabelNumber,           label->getNumberLabel()},
//                                                                      {ControllerEventsArgs::LabelCreationTimeMs,   (qlonglong)label->getTimeStartLabelMS()}  }));
//  }
//  else
//  {
//    qDebug() << "none";
//  }
}

void MonitorController::deleteLabel()//доделать
{
  mLabelManagerGlobal->deleteLabel();
}

int MonitorController::getLabelsCountPerCurrentSession() const
{
  if (!mLabelManagerGlobal) { return 0; }
  return mLabelManagerGlobal->getLabelsCountPerCurrentSession();
}

LabelManager *MonitorController::getLabelManager() const
{
  return mLabelManagerGlobal;
}
void MonitorController::softwareStorageUnavailable()
{
  qDebug() << "Software Storage Unavailable";
  mSoftwareStorageState = ControllerEvent::SoftwareStorageUnavailable;
  emit controllerEvent(mSoftwareStorageState);
}

bool MonitorController::setLevelsAndStatesAlarm(int lowLevelAlarm, int highLevelAlarm, bool lowEnabel, bool highEnable)
{
  if ((!mICPSettings) || (lowLevelAlarm >= highLevelAlarm)) { return false; }

  const int LLA = mICPSettings->getLowLevelAlarm();
  const int HLA = mICPSettings->getHighLevelAlarm();
  const bool LLSA = mICPSettings->getLowLevelStateAlarm();
  const bool HLSA = mICPSettings->getHighLevelStateAlarm();

  // Если одно из значений уровня тревоги изменилось
  if ((LLA != lowLevelAlarm) || (HLA != highLevelAlarm))
  {
    mICPSettings->setLowLevelAlarm(lowLevelAlarm);
    mICPSettings->setHighLevelAlarm(highLevelAlarm);
    emit controllerEvent(ControllerEvent::UpdatedAlarmLevels);
  }

  if ((LLSA != lowEnabel) || (HLSA != highEnable))
  {
    mICPSettings->setLowLevelStateAlarm(lowEnabel);
    mICPSettings->setHighLevelStateAlarm(highEnable);
    emit controllerEvent(ControllerEvent::UpdatedAlarmStates);
  }
  mICPSettings->writeAlarmSettings();
  return true;
}
bool MonitorController::setPressureUnits(uint8_t mCurrentPressureUnitsIndex)
{
  if (!mICPSettings) { return false; }
  if (mICPSettings->getCurrentPressureIndex() != mCurrentPressureUnitsIndex)
  {
    mICPSettings->setCurrentPressureUnits(mCurrentPressureUnitsIndex);
    emit controllerEvent(ControllerEvent::ChangePressureUnits);
    mICPSettings->writeCurrentSensorReadingsSettings();
    return true;
  }
  return false;
}
bool MonitorController::setAverageInterval(float mAverageIntervalSec)
{
  if (!mICPSettings) { return false; }
  if (mICPSettings->getCurrentAverageIntervalSec() != mAverageIntervalSec)
  {
    mICPSettings->setCurrentAverageIntervalSec(mAverageIntervalSec);
    mICPSettings->writeCurrentSensorReadingsSettings();
  }
  return true;
}

bool MonitorController::setInetrvalsOnGraph(float mCurrentReadingsGraphIntervalX, float mCurrentReadingsGraphIntervalYLow,
                                            float mCurrentReadingsGraphIntervalYHigh, float mTickCountX, float mTickCountY)
{
  if (!mICPSettings) { return false; }
  const float CXR = mICPSettings->getCurrentReadingsGraphIntervalX();
  float CYRL = mICPSettings->getCurrentReadingsGraphIntervalYLow();
  float CYRH = mICPSettings->getCurrentReadingsGraphIntervalYHigh();
  const float TCX = mICPSettings->getCurrentTickCountX();
  float TCY = mICPSettings->getCurrentTickCountY();

  // Если одно из значений осей изменилось
  if ((CXR  != mCurrentReadingsGraphIntervalX) || (CYRL != mCurrentReadingsGraphIntervalYLow) || (CYRH != mCurrentReadingsGraphIntervalYHigh))
  {
    mICPSettings->setCurrentReadingsGraphIntervalX(mCurrentReadingsGraphIntervalX);
    mICPSettings->setCurrentReadingsGraphIntervalY(mCurrentReadingsGraphIntervalYLow, mCurrentReadingsGraphIntervalYHigh);
    emit controllerEvent(ControllerEvent::UpdateGraphIntervals);
  }
  if ((TCX != mTickCountX) || (TCY != mTickCountY))
  {
    mICPSettings->setCurrentTickCountX(mTickCountX);
    mICPSettings->setCurrentTickCountY(mTickCountY);
    emit controllerEvent(ControllerEvent::UpdateGraphTicks);
  }
  if ((CXR  != mCurrentReadingsGraphIntervalX) || (CYRL != mCurrentReadingsGraphIntervalYLow)
      || (CYRH != mCurrentReadingsGraphIntervalYHigh) || (TCX != mTickCountX) || (TCY != mTickCountY))
  {
    mICPSettings->writeCurrentSensorReadingsSettings();
  }
  return true;
}
bool MonitorController::setGeneralParam(float mFontScaleFactor)
{
  if (!mICPSettings) { return false; }
  const float FSF = mICPSettings->getFontScaleFactor();
  const QString SSUUID= mICPSettings->getSoftwareStorageUUID();

  // Если одно из значений осей изменилось
  if (FSF  != mFontScaleFactor)
  {
    mICPSettings->setFontScaleFactor(mFontScaleFactor);
    emit controllerEvent(ControllerEvent::UpdateGeneralGroupInfo);
  }
  // Сохраняем настройки
  mICPSettings->writeGeneralSettings();

  return true;
}


void MonitorController::processAlarmEvent(AlarmEvent event, const QVariantMap &args)
{
  switch (event)
  {
    case AlarmEvent::Low:
    {
      emit controllerEvent(ControllerEvent::AlarmLowOn, args);
      break;
    }
    case AlarmEvent::Normal:
    {
      emit controllerEvent(ControllerEvent::AlarmOff, args);
      break;
    }
    case AlarmEvent::High:
    {
      emit controllerEvent(ControllerEvent::AlarmHighOn, args);
      break;
    }
    default: break;
  }
}


