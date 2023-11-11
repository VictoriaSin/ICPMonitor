#include "gui/mainwindow.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>

#include "version.h"
#include "controller/monitorcontroller.h"
#include "controller/settings.h"

#include "sensor/sensor_enums.h"
#include "controller/controller_enums.h"

#include "global_functions.h"

#include "unistd.h"
#include "global_define.h"
#include "controller/settings.h"
#include "clock.h"

#include "../core/sensor/zsc.h"

QString mntDirectory("/media/ICPMonitor");
enum MOUNT_MESSAGE
{
  OK,
  ERROR_CREATED_DIRECTORY,
  ERROR_CREATED_MNT_POINT,
  ERROR_FLASH_ABSEND,
};

__inline bool findFlashDevices(QStringList *mDevicesByUUIDList)
{
  QStringList cmdAllDevList = executeAConsoleCommand("ls", QStringList() << "-l" << "/dev/disk/by-uuid").split("\n");
  uint8_t tempIndex = 0;
  for (uint8_t i = 1; i< cmdAllDevList.count()-1; i++)
  {
    tempIndex = cmdAllDevList[i].split(" ").indexOf("->");
    if (!(cmdAllDevList[i].split(" ")[tempIndex-1].contains("-"))) //ntfs
    {
      mDevicesByUUIDList->append(cmdAllDevList[i].split(" ")[tempIndex-1] + " " + cmdAllDevList[i].split(" ")[tempIndex+1].split("/")[2]);
      qDebug() << "Flash: " << cmdAllDevList[i].split(" ")[tempIndex-1] + " " + cmdAllDevList[i].split(" ")[tempIndex+1].split("/")[2];
    }
  }
  if (mDevicesByUUIDList->isEmpty())
  {
    return false;
  }
  return true;
}
__inline void removeMountsPoints(/*QStringList *Devices*/)
{
  QStringList temp = executeAConsoleCommand("df", QStringList() << "--output=source,fstype,target").split("\n");
  qDebug("FINDPROCMOUNTS");
  for (int32_t i=1; i < temp.count(); i++)
  {
    if (temp[i].contains("fuseblk") || temp[i].contains("ntfs3") || temp[i].contains("vfat"))
    {
      QStringList removeDev = temp[i].simplified().split(" ");
      if (removeDev[2].length()>8)
      {
#ifdef PC_BUILD
        if (executeAConsoleCommand("sudo", QStringList() << "umount" << removeDev[2]) != "")
#else
        if (executeAConsoleCommand("umount", QStringList() << removeDev[2]) != "")
#endif
        {
          qDebug() << "umount" << removeDev[2];
          return;
        }
      }
    }
  }
}
__inline bool isMntDirectoryCreated()
{
  QDir mICPReadingsDir(mntDirectory);
  return mICPReadingsDir.exists();
}
__inline QString findMountRecordInProcMounts()
{
  QStringList mounts = executeAConsoleCommand("cat", QStringList() << "/proc/mounts").split("\n");
  for (uint8_t i=0; i<mounts.count(); i++)
  {
    if (mounts[i].contains(mntDirectory))
    {
      qDebug() << "Find mount point " << mounts[i] ;
      return mounts[i].split(" ")[0];
    }
  }
  return "";
}
__inline bool findUUIDInDevicesList(QString *UUID, QStringList mDevicesByUUIDList, QString *rasdel)
{
  for (uint8_t i=0; i < mDevicesByUUIDList.count(); i++)
  {
    if (mDevicesByUUIDList[i].contains(*UUID))
    {
      *rasdel = mDevicesByUUIDList[i].split(" ")[1];
      return true;
      //break;
    }
  }
  return false;
}
__inline bool tryCreateMountPoint(QString *rasdel)
{
#ifdef PC_BUILD
  QString result = executeAConsoleCommand("sudo", QStringList() << "mount" << "/dev/"+*rasdel << mntDirectory);
#else
  QString result = executeAConsoleCommand("mount", QStringList() << "/dev/"+*rasdel << mntDirectory);
#endif
  if (result == "")
  {
    qDebug() << "Created mount point" << "/dev/"+*rasdel << mntDirectory;
    return true;
  }
  else
  {
    qDebug() << "Error created mount point" << "mount /dev/"+*rasdel << mntDirectory;
  }
  return false;
}

u8 mount(QString *UUID, QString *rasdel)
{
  QStringList Devices;
  QStringList mDevicesByUUIDList;
  QString newRasdel;
  if (!findFlashDevices(&mDevicesByUUIDList)) {return MOUNT_MESSAGE::ERROR_FLASH_ABSEND; } // ищем все флешки
  removeMountsPoints(/*&Devices*/);
  // всегда монтируем в папку /media/ICPMonitor = mntDirectory
  if (isMntDirectoryCreated()) // проверяем есть ли уже такая директория
  {
    qDebug("MntDirectoryCreated isExist");
    if (*UUID != "")
    {
      if ((newRasdel = findMountRecordInProcMounts()) != "") // допустим папка есть, но данных нет (ну не записали туда ничего еще !!!) - проверяем смонтирована ли флешка в записях файла /proc/mounts
      {
        qDebug() << "new" << newRasdel;
        *UUID = executeAConsoleCommand("lsblk", QStringList() << "-o" << "UUID" << newRasdel).split("\n")[1];
        *rasdel = newRasdel;
        qDebug() << *rasdel;
        return MOUNT_MESSAGE::OK;
      }
    }
  }
  else
  {
    qDebug() << "Try created directory" << mntDirectory;
    QString outputString = executeAConsoleCommand("mkdir", QStringList() << "-m" << "777" << mntDirectory);
    if (outputString != "")
    {
      qDebug() << "Directory" << mntDirectory << "not created"; return MOUNT_MESSAGE::ERROR_CREATED_DIRECTORY;
      qDebug() << "Result" << outputString;
    }
  }

  if (*UUID == "")
  {
    qDebug("findUUIDInDevicesList");
    if (findUUIDInDevicesList(UUID, mDevicesByUUIDList, rasdel))
    {
      if (tryCreateMountPoint(rasdel)) { return MOUNT_MESSAGE::OK; }
    }
  }
  for (u8 i = 0; i < mDevicesByUUIDList.count(); i++)
  {
    if ((QString *)&(mDevicesByUUIDList[i].split(" ")[1]))
    {
      *UUID = mDevicesByUUIDList[i].split(" ")[0];
      *rasdel = mDevicesByUUIDList[i].split(" ")[1];
      if (tryCreateMountPoint(&(mDevicesByUUIDList[i].split(" ")[1])))
      {
        return MOUNT_MESSAGE::OK;
      }
    }

  }
  return MOUNT_MESSAGE::ERROR_CREATED_MNT_POINT;
}

bool umount(QString *rasdel)
{
#ifndef PC_BUILD
  if (executeAConsoleCommand("umount", QStringList() << "/dev/" + *rasdel) != "")// "/dev/sdc1") != "")
  {
    qDebug() << "not umount" << mntDirectory;
    return false;
  }
  else
  {
    qDebug() << "umount" << mntDirectory;
    QString outRes = executeAConsoleCommand("rm", QStringList() << "-R" << mntDirectory);
    if (outRes == "")
    {
      qDebug() << "Directory" << mntDirectory << "deleted";
      return true;
    }
    else
    {
      qDebug() << outRes;
    }
  }
#endif
  return false;
}

QString convertDateTimeToString(u8 *data)
{
  char strOut[150];
  sprintf(strOut, "20%02x-%02x-%02x %02x:%02x:%02x",
      data[6],
      data[5],
      data[4],
      data[1],
      data[2],
      data[0]);
  return QString(strOut);
}

bool initFlash(QString currRasdel)
{
  QStringList temp = executeAConsoleCommand("cat", QStringList() << "/proc/mounts").split("\n");
  for (int i=0; i< temp.count(); i++)
  {
    //if (temp[i].split(" ")[0].contains(currRasdel))
    if (temp[i].simplified().split(" ")[0].contains(currRasdel))
    {
      if(!isMntDirectoryCreated())
      {
        qDebug() << "Try created directory" << mntDirectory;
        QString outputString = executeAConsoleCommand("mkdir", QStringList() << "-m" << "777" << mntDirectory);
        if (outputString != "")
        {
          qDebug() << "Directory" << mntDirectory << "not created"; return MOUNT_MESSAGE::ERROR_CREATED_DIRECTORY;
          qDebug() << "Result" << outputString;
        }
      }
      else
      {qDebug() << mntDirectory << "Try created directory" ;}
      QString result = executeAConsoleCommand("mount", QStringList() << currRasdel << mntDirectory);
      if (result != "")
      {
        qDebug() << result;
        return 10;
      }
      return 0;
    }
  }
  return 11;
}

uint16_t inputData[36] = {12979, 13722, 15682, 17898, 20345, 21989, 22926, 22695, 22975, 22865,
                       22963, 23072, 23024, 22549, 22452, 22585, 21965, 21758, 21587, 20942,
                       20345, 19676, 18896, 18701, 17618, 17082, 16206, 15658, 15231, 14708,
                       13965, 13868, 13235, 12869, 13113, 14391};

uint16_t outData[360] ;
void divArr()
{
    u16 start, stop;
    for (u8 i = 0; i < 35; i++)
    {
        start = inputData[i];
        stop = inputData[i+1];
        i16 step = (stop - start)/10;
        outData[i*10] = start;
        for(u8 k = 1; k < 10; k++)
        {
            outData[i*10 + k] = start + step * k;
        }

    }
    outData[339] = 14391;

    for (u16 i = 0; i < 340; i++)
    {
        qDebug() << outData[i] << ",";
    }
}

Settings *mICPSettings {nullptr};
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  //divArr();
  //exit(1);


  // fbset --geometry 720 480 720 480 16 --timings 37037 60 16 30 9 62 6
  QString ttt = executeAConsoleCommand("fbset", QStringList() << "--geometry" << "720" << "480" << "720" << "480" << "16" << "--timings" << "37037" << "60" << "16" << "30" << "9" << "62" << "6");
  //qDebug() << ttt;

  // Получение настроек из контроллера
#ifdef PC_BUILD
  mICPSettings = new Settings("ICPMonitorSettings.ini");
#else
  mICPSettings = new Settings("/opt/ICPMonitor/bin/ICPMonitorSettings.ini");
#endif
  mICPSettings->registrateLangFile(QLocale::Language::English, ":/trans/core_en.qm");
  mICPSettings->registrateLangFile(QLocale::Language::English, ":/trans/icp_monitor_en.qm");
  // Чтение настроек
  mICPSettings->readAllSetting();

  QString currUUID = mICPSettings->getSoftwareStorageUUID();
  //qDebug() << currUUID;
  QString currRasdel = mICPSettings->getFlashDeviceMountPart();
  //qDebug() << currRasdel;
  mount(&currUUID, &currRasdel);
  mICPSettings->setSoftwareStorageUUID(currUUID);
  mICPSettings->writeAllSetting();
  Q_INIT_RESOURCE(core_res);
  // Игнорируемые события тача автоматически переопределять в MouseEvent
  QCoreApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, true);

  //qRegisterMetaType<SensorEvent>();
  //qRegisterMetaType<FileControllerEvent>();


  qRegisterMetaType<ControllerEvent>();
  //qRegisterMetaType<BlockDeviceManagerEvent>();

  const QString APP_NAME = "ICPMonitor";
  const QString ORG_NAME = "CORESAR";
  QCoreApplication::setApplicationName(APP_NAME);
  QCoreApplication::setOrganizationName(ORG_NAME);
  QCoreApplication::setApplicationVersion(version::VERSION_STRING2);

#ifdef RELEASE_BUILD
  uint8_t clockBuffer[7];
  if (getRTC(clockBuffer) == I2C_RESULT::I2C_OK)
  {
    qDebug() << "Date/Time" << convertDateTimeToString(clockBuffer);
    if (setRTC(clockBuffer) != I2C_RESULT::I2C_OK) { exit(66); }
    setDateTime(clockBuffer);
  }
  else
  {
    exit(66);
  }
#endif

  MainWindow w;
  w.show();
  const int exitCode = a.exec();
  w.hide();
  umount(&currRasdel);
  qDebug() << "Exit" << exitCode;
  return exitCode;
}

