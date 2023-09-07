#ifndef USB_FUNCS_H
#define USB_FUNCS_H

#include <vector>

#include <QString>

/*! Информация о разделе флешки */
struct InfoAboutThePartOfFlashDrive {
    /*! Имя основного (большего) раздела USB */
    QString drivePartitionName;

    /*! Размер раздела USB */
    uint64_t partSize = 0;

    /*! Точка монтирования раздела */
    QString partMountPoint;

    InfoAboutThePartOfFlashDrive(const QString &drivePartitionName, uint64_t partSize,
                                    const QString &partMountPoint) {
        this->drivePartitionName = drivePartitionName;
        this->partSize = partSize;
        this->partMountPoint = partMountPoint;
    }
};

/*! Монтирование флешки
    mountPoint - место для монтирования флешки
    flashPath - путь к блочному устройству
*/
void mountUSB(const QString& mountPoint, const QString& flashPath);

/*! Размонтирование флешки
    umountPoint - путь, который необходимо размонтировать
*/
void umountUSB(const QString& umountPoint);

/*! Возвращает путь монтирования раздела флешки
    Если раздел флешки не смонтирован, возвращается пустая строка QString()
    flashPart - раздел флешки, для которой ищем точку монтирования
 */
QString mountPointOfFlashDrive(const QString& flashPart);

/*! Получаение всех разделов всех флешек, подключенных к ПК */
std::vector<InfoAboutThePartOfFlashDrive> getAllSDNamesWithTheirParts();

/*! Возвращает имена наибольших разделов установленных флешек.
    usbs - массив флешек
    Если флешки не найдены, то возвращается пустой список.
 */
std::vector<InfoAboutThePartOfFlashDrive> getSDNamesOfMaxPart(const std::vector<InfoAboutThePartOfFlashDrive>& usbs);

/*! Возвращает максимальные разделы каждой флешки в системе */
std::vector<InfoAboutThePartOfFlashDrive> getMaxPartOfFlashsInSystem();



#endif // USB_FUNCS_H
