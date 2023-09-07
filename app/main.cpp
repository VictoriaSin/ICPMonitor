#include "gui/mainwindow.h"

#include <QApplication>
#include <QThread>
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
// добавить функцию демонтирования


enum MOUNT_MESSAGE
{
  OK,
  ERROR_CREATED_DIRECTORY,
  ERROR_CREATED_MNT_POINT,
  ERROR_FLASH_ABSEND,
};

__inline bool findFlashDevices(QStringList *mDevicesByUUIDList)
{
    QStringList cmdAllDevList = executeAConsoleCommand("ls", QStringList() << "/dev/disk/by-uuid" << "-l").split("\n");
    uint8_t tempIndex = 0;
    qDebug("FINDFLASHDEVICES");
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
    //qDebug() << *mDevicesByUUIDList;//UUID+" "+раздел
}

__inline bool findProcMounts(QStringList *Devices)
{
    QStringList temp = executeAConsoleCommand("df", QStringList() << "--output=source,fstype,target").split("\n");
    qDebug("FINDPROCMOUNTS");
    for (uint8_t i=1; i<temp.count(); i++)
    {
        if (temp[i].contains("fuseblk") || temp[i].contains("ntfs3"))
        {
            Devices->append(temp[i]);
            qDebug() << temp[i];
        }
    }
}

__inline bool isMntDirectoryCreated(QDir *mICPReadingsDir)
{
    return mICPReadingsDir->exists();
}

__inline QString findMountRecordInProcMounts(QStringList mDevicesByUUIDList)
{
    QStringList mounts = executeAConsoleCommand("cat", QStringList() << "/proc/mounts").split("\n");
    for (uint8_t i=0; i<mounts.count(); i++)
    {
        if (mounts[i].contains("/media/ICPMonitor"))
        {

            qDebug() << "Find mount point " << mounts[i] ;
            //break;
            return mounts[i].split(" ")[0];
        }
    }
    qDebug() << "No find mount point ";
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
    qDebug() << "try create mount point" << "/dev/"+*rasdel << "/media/ICPMonitor";
    if (executeAConsoleCommand("mount", QStringList() << "/dev/"+*rasdel << "/media/ICPMonitor") == "")
    {
        return true;
    }

    return false;
}

u8 mount(QString *UUID)
{
  QStringList Devices;
  QStringList mDevicesByUUIDList;
  QString rasdel = "";
  bool isMounted = false;
  QDir mICPReadingsDir("/media/ICPMonitor/");
  QString newRasdel;
  if (!findFlashDevices(&mDevicesByUUIDList)) {return MOUNT_MESSAGE::ERROR_FLASH_ABSEND; } // ищем все флешки
  findProcMounts(&Devices);
 //  // всегда монтируем в папку /media/ICPMonitor/
  if (isMntDirectoryCreated(&mICPReadingsDir)) // проверяем есть ли уже такая директория
  {
      qDebug("Dir not empty");
      if (*UUID != "")
      {
          qDebug("UUID not empty");
          if ((newRasdel = findMountRecordInProcMounts(mDevicesByUUIDList)) != "") // допустим папка есть, но данных нет (ну не записали туда ничего еще !!!) - проверяем смонтирована ли флешка в записях файла /proc/mounts
          {
              qDebug() << newRasdel;
              *UUID = executeAConsoleCommand("lsblk", QStringList() << "-o" << "UUID" << newRasdel).split("\n")[1];
              return MOUNT_MESSAGE::OK;
          }
      }
  }
  else
  {
      qDebug("Create dir");
      //if (!(mICPReadingsDir.mkdir(mICPReadingsDir.path()))) {return MOUNT_MESSAGE::ERROR_CREATED_DIRECTORY;}
      if (executeAConsoleCommand("mkdir", QStringList() << "-m" << "777" << mICPReadingsDir.path()) != "") {return MOUNT_MESSAGE::ERROR_CREATED_DIRECTORY;};
  }

  if (*UUID == "")
  {
      qDebug("findUUIDInDevicesList");
      if (findUUIDInDevicesList(UUID, mDevicesByUUIDList, &rasdel))
      {
          if (tryCreateMountPoint(&rasdel)) { return MOUNT_MESSAGE::OK; }
      }
  }
  for (u8 i = 0; i < mDevicesByUUIDList.count(); i++)
  {
    //*UUID = mDevicesByUUIDList[i].split(" ")[0];//setUUIDFromList(i);
    //if (tryCreateMountPoint()) { saveInitFile(); return MOUNT_MESSAGE::OK; }
    if ((QString *)&(mDevicesByUUIDList[i].split(" ")[1]))
    {
        *UUID = mDevicesByUUIDList[i].split(" ")[0];
        if (tryCreateMountPoint(&(mDevicesByUUIDList[i].split(" ")[1])))
        {
        return MOUNT_MESSAGE::OK;
        }
    }

  }
  return MOUNT_MESSAGE::ERROR_CREATED_MNT_POINT;
}


bool umount()
{
    //qDebug() << executeAConsoleCommand("rm", QStringList() << "-R" << "/media/ICPMonitor/");
    if (executeAConsoleCommand("rm", QStringList() << "-R" << "/media/ICPMonitor") == "")
    {
        if (executeAConsoleCommand("umount", QStringList() << "/dev/sdc1") != "")
        {
            qDebug() << "not umount";
            return false;
        }
        qDebug() << "umount";
        return true;
    }
    return false;
}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MonitorController monitorController;
    Settings *settings = monitorController.settings();
    settings->readAllSetting();
    QString currUUID = settings->getSoftwareStorageUUID();
    int errCode = mount(&currUUID);
    qDebug() << errCode;
    if (errCode != 0) { exit(7); };
    settings->setSoftwareStorageUUID(currUUID);
    settings->writeAllSetting();
    //exit(10);
    Q_INIT_RESOURCE(core_res);

    // Игнорируемые события тача автоматически переопределять в MouseEvent
    QCoreApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, true);

    //bufferRecord_1.currentPos = 0;

    qRegisterMetaType<SensorEvent>();
    qRegisterMetaType<ControllerEvent>();
    qRegisterMetaType<FileControllerEvent>();
    qRegisterMetaType<BlockDeviceManagerEvent>();

    const QString APP_NAME = "ICPMonitor";
    const QString ORG_NAME = "CORESAR";
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setOrganizationName(ORG_NAME);
    QCoreApplication::setApplicationVersion(version::VERSION_STRING2);

    qDebug() << QString("%1 v%2").arg(APP_NAME, version::VERSION_STRING);

    // Создание GUI
    MainWindow w;

    // Создание контроллера приложения и его потока
    QThread mControllerThread;
//    MonitorController monitorController;

    // Получение настроек из контроллера
//    Settings *settings = monitorController.settings();

    // Регистрация файлов перевода
    settings->registrateLangFile(QLocale::Language::English, ":/trans/core_en.qm");
    settings->registrateLangFile(QLocale::Language::English, ":/trans/icp_monitor_en.qm");

    // Чтение настроек
//    settings->readAllSetting();

    // Установка контроллера виджетам
    w.installController(&monitorController);

    // Инициализация контроллера и сброс контроллера в поток
    monitorController.init();
    monitorController.moveToThread(&mControllerThread);
    mControllerThread.start();

    // Запуск виджетов
    w.show();
    const int exitCode = a.exec();

    // завершение работы контроллера, выполняемое в потоке контроллера
    QTimer::singleShot(0, &monitorController, [controller = &monitorController]()
    {
        controller->terminate();
    });

    mControllerThread.quit();
    mControllerThread.wait(10000);

    qDebug() << "Exit" << exitCode;
    sleep(1);
    umount();
    return exitCode;
}


//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <linux/i2c-dev.h>
//#include <linux/gpio.h>
//#include "owi_protocol.h"

//#define DEV_ADDRESS 0x68
//#define DEV_REGISTERS_NUM 7
//const char *devName = "/dev/i2c-2";
//bool getRTC()
//{
//    char buf[10];
//    // Open up the I2C bus
//    int file = open(devName, O_RDWR);
//    if (file == -1)
//    {
//        qDebug() << "Error open " << devName;
//        return false;
//    }
//    // Specify the address of the slave device.
//    if (ioctl(file, I2C_SLAVE, DEV_ADDRESS) < 0)
//    {
//        qDebug("Failed to acquire bus access and/or talk to slave");
//        return false;
//    }

//    // Write a byte to the slave.
//    buf[0] = 0;
//    if (write(file, buf, 1) != 1)
//    {
//        qDebug("Failed to write to the i2c bus");
//        return false;
//    }

//    // Read a byte from the slave.
//    if (read(file,buf,DEV_REGISTERS_NUM) != DEV_REGISTERS_NUM)
//    {
//        qDebug("Failed to read from the i2c bus");
//        return false;
//    }
//    //for(uint8_t i=0; i<6; i++)
//    char strOut[100];
//    sprintf(strOut, "Date/Time %02x/%02x/20%02x %02x:%02x:%02x", buf[4], buf[5], buf[6], buf[2], buf[1], buf[0]);
//    qDebug() << strOut;//<< "year=20" <<  << buf[5]<< buf[4]<< buf[3]<< buf[2];
//    return true;
//}
//bool setRTC()
//{
//    char buf[10];
//    const char *devName = "/dev/i2c-2";

//    // Open up the I2C bus
//    int file = open(devName, O_RDWR);
//    if (file == -1)
//    {
//        qDebug() << "Error open " << devName;
//        return false;
//    }

//    // Specify the address of the slave device.
//    if (ioctl(file, I2C_SLAVE, DEV_ADDRESS) < 0)
//    {
//        qDebug("Failed to acquire bus access and/or talk to slave");
//        return false;
//    }

//    // Write a byte to the slave.
//    buf[0] = 0;
//    buf[1] = 0x22;
//    buf[2] = 0x11;
//    buf[3] = 0x03;
//    buf[4] = 0x23;
//    buf[5] = 0x08;
//    buf[6] = 0x23;
//    if (write(file, buf, DEV_REGISTERS_NUM) != DEV_REGISTERS_NUM)
//    {
//        qDebug("Failed to write to the i2c bus");
//        return false;
//    }
//    return true;
//}
//QString outString = executeAConsoleCommand("cat", QStringList() << "/sys/kernel/debug/gpio");
//QStringList parser = outString.split('\n');
//for (const auto &line : parser)
//{
//    QStringList temp = line.split(' ');
//    u8 cnt = temp.count();
//    if (cnt > 1)
//    {
//    if (temp.at(1) == "gpio-25") {qDebug() << "25 find";  }
//    if (temp.at(1) == "gpio-26") {qDebug() << "26 find";  }
//    }
//}





// Формат хранения данных значений [TIME_OFFSET(4bytes)] [RAW_DATA(2bytes)] = 6bytes (LEN_RECORD)
// currentBuffer = 1 or 2
// 2 Буфера BUF_1[N * LEN_RECORD] BUF_2[N * LEN_RECORD]
// Записали запись currentPos++
// if (currentPos == MAX_CNT_RECORD)
// {
//  if (currentBufferRecord == 1)  {currentBufferRecord = 2;}
//  else {currentBufferRecord = 1; }
//  sendEvent(needSaveBaseRecords);
// }
// Папка по времени и дате записи
// Отдельно файл записи
// файл для заметок
// файл для интервалов
// сырые данные



//#define MAX_CNT_RECORD 300
//typedef struct
//{
//  uint32_t timeOffset;
//  uint16_t rawData;
//}_recordFields;

//typedef struct
//{
//   _recordFields field[MAX_CNT_RECORD];
//   uint16_t currentPos;
//}_bufferRecord;

//uint8_t currentBufferRecord = 1;
//_bufferRecord bufferRecord_1, bufferRecord_2;
