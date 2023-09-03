#ifndef BLOCKDEVICESSELECTIONDIALOG_H
#define BLOCKDEVICESSELECTIONDIALOG_H

#include <memory>

#include <QMap>
#include <QIcon>

#include "selectiondialog.h"

class BlockDevice;

class BlockDevicesSelectionDialog : public SelectionDialog
{
    Q_OBJECT

public:
    /*! Выводимая информация об устройстве */
    enum InfoBlockDevice : uint8_t {
        UUID = 0x1,
        DevName = 0x2,
        Vendor = 0x4,
        Model = 0x8,
    };

    Q_DECLARE_FLAGS(InfoBlockDevices, InfoBlockDevice);

private:
    /*! Карта блочных устройств, связанных с кнопками
        key - id кнопки, к которой привязано блочное устройство;
        value - блочное устройство.
    */
    QMap<int, std::shared_ptr<BlockDevice>> mBlockDevices;

    /*! Максимальное кол-во блочных устройств в ряду */
    const int MaxBlockDevicesInRow {3};

    /*! Выводимая информация об устройстве */
    InfoBlockDevices mInfoBlockDevices {UUID | DevName | Vendor | Model};

    /*! Размер шрифта */
    const int FontSize {15};

    /*! Изображение в кнопке */
    const QIcon ToolButtonIcon {":/icons/flashDrive.svg"};

    /*! Размер кнопки */
    const QSize iconSize {125, 75};

public:
    BlockDevicesSelectionDialog(QWidget *parent = nullptr);

    /*! Установка блочных устройств на диалоговое окно */
    void setBlockDevices(const QList<std::shared_ptr<BlockDevice>> &blockDevices);

    /*! Удаляет блочные устройства с диалогового окна */
    void removeBlockDevices();

    /*! Установка выводимой информации о блочном устройстве */
    void setInfoFlags(InfoBlockDevices flags);

    /*! Возвращает выбранное блочное устройство
        Если была нажата кнопка reject или не выбрано
        блочное устройство, то вернёт nullptr.
    */
    std::shared_ptr<BlockDevice> getSelectedBlockDevice();

private:
    /*! Генерация информации об устройстве
        в зависимости от выставленных флагов mFlags
    */
    QString genInfoAboutBlockDev(const std::shared_ptr<BlockDevice> &dev);

    // AbstractDialogWindow interface
public:
    void retranslate() override;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(BlockDevicesSelectionDialog::InfoBlockDevices)

#endif // BLOCKDEVICESSELECTIONDIALOG_H
