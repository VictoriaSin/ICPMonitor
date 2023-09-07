#include "usb_funcs.h"
#include "global_functions.h"

#include <QDebug>

QDebug operator<<(QDebug debug, const InfoAboutThePartOfFlashDrive &val)
{
    QDebugStateSaver saver(debug);
    debug.space() << "Name:" << val.drivePartitionName << "Size:" << QString::number(val.partSize) << "MountPoint:" << val.partMountPoint;;

    return debug;
}

void mountUSB(const QString &mountPoint, const QString &flashPath)
{
    executeAConsoleCommand("mount", QStringList() << flashPath << mountPoint);
}

void umountUSB(const QString &umountPoint)
{
    executeAConsoleCommand("umount", QStringList() << umountPoint);
}

bool isMounted(const InfoAboutThePartOfFlashDrive &flash)
{
    return flash.partMountPoint != "";
}

QString mountPointOfFlashDrive(const QString &flashPart)
{
    auto allFlashDrives(getAllSDNamesWithTheirParts());

    auto it = std::find_if(allFlashDrives.begin(), allFlashDrives.end(), [flashPart](InfoAboutThePartOfFlashDrive infoFD){
        return infoFD.drivePartitionName == flashPart;
    });

    if (it != allFlashDrives.end()) {
        return it.base()->partMountPoint;
    }

    return QString();
}

std::vector<InfoAboutThePartOfFlashDrive> getAllSDNamesWithTheirParts()
{
    std::vector<InfoAboutThePartOfFlashDrive> allPartsOfUSB;

    auto usbDevicesInfo = executeAConsoleCommand("bash", QStringList() << "-c" << "lsblk -l -b -o NAME,SIZE,MOUNTPOINT | awk '/sd[a-z][[:digit:]]/ {print}'").split("\n");

    // Удаляем лишние пробелы, escape-последовательности и пустые строки
    // Делим каждый раздел диска на три составляющие: имя раздела, емкость, путь монтирования
    for (int i = 0; ; ++i) {
        if (i >= usbDevicesInfo.size()) {
            break;
        }

        usbDevicesInfo[i] = usbDevicesInfo[i].simplified();

        if (usbDevicesInfo[i] == "") {
            usbDevicesInfo.removeAt(i);
            i -= 1;
            continue;
        }

        const auto temp = usbDevicesInfo[i].split(" ");

        if (temp.size() >= 3) {
            QString mountPoint; // Возможно путь монтирования был с пробелом
            for (int i = 2; ; ++i) {
                if (i == temp.size() - 1)
                {
                    mountPoint += temp[i];
                    break;
                }
                mountPoint += temp[i] + " ";
            }
            allPartsOfUSB.push_back(InfoAboutThePartOfFlashDrive(temp[0], temp[1].toULongLong(), mountPoint));
        } else if (temp.size() == 2) {
            allPartsOfUSB.push_back(InfoAboutThePartOfFlashDrive(temp[0], temp[1].toULongLong(), ""));
        }
    }

    return allPartsOfUSB;
}

std::vector<InfoAboutThePartOfFlashDrive> getSDNamesOfMaxPart(const std::vector<InfoAboutThePartOfFlashDrive> &usbs)
{
    // Храним тут наибольший раздел каждой флешки
    std::vector<InfoAboutThePartOfFlashDrive> maxPartOfUSBs;

    uint64_t currentMaxPartSize = 0;
    QString baseNameSD = "*/?#$%^!";
    QString currentSDPart = "*/?#$%^!";
    auto maxPartIterator = usbs.cbegin();

    // Поиск разделов с максимальным объёмом памяти
    for (auto it = usbs.cbegin(); it != usbs.cend(); ++it) {
        // Если базовое имя флешки совпадает
        if (it->drivePartitionName.contains(baseNameSD)) {
            // Если размер раздела больше прошлого раздела
            if (currentMaxPartSize < it->partSize) {
                currentSDPart = it->drivePartitionName;
                currentMaxPartSize = it->partSize;
                maxPartIterator = it;
            }
        } else {
            // Если базовое имя флешки не совпало, то добавляем максимальный раздел предыдущей флешки
            if (it != usbs.cbegin()) {
                maxPartOfUSBs.push_back(usbs.at(maxPartIterator - usbs.cbegin()));
            }

            baseNameSD = QString(it->drivePartitionName).remove(QRegExp("\\d+"));
            currentSDPart = it->drivePartitionName;
            currentMaxPartSize = it->partSize;
            maxPartIterator = it;

            // Если найден всего один раздел
            if (usbs.size() == 1) {
                maxPartOfUSBs.push_back(usbs.at(maxPartIterator - usbs.cbegin()));
                break;
            }
        }

        // Если раздел был последним
        if (it == usbs.cend() - 1) {
            maxPartOfUSBs.push_back(usbs.at(maxPartIterator - usbs.cbegin()));
        }
    }

    return maxPartOfUSBs;
}

std::vector<InfoAboutThePartOfFlashDrive> getMaxPartOfFlashsInSystem()
{
    return getSDNamesOfMaxPart(getAllSDNamesWithTheirParts());
}
