#include "blockdevicesselectiondialog.h"
#include "blockDevices/blockdevice.h"
#include <QDebug>
#include <QPushButton>
#include <QButtonGroup>

BlockDevicesSelectionDialog::BlockDevicesSelectionDialog(QWidget *parent) :
    SelectionDialog(parent)
{
    // Делаем шрифт для текста поменьше, чем у родителя
    mFontForText.setPointSize(FontSize);
}

void BlockDevicesSelectionDialog::setBlockDevices(const QList<std::shared_ptr<BlockDevice> > &blockDevices)
{
    // Удаляем с диалогового окна ранее показанные блочные устройства
    removeBlockDevices();

    // Добавляем на диалоговое окно
    int numButton = 0;
    for (const auto& dev : qAsConst(blockDevices)) {
        // Если нет устройства или оно не валидно
        if (!dev || !dev->isValid()) {
            return;
        }

        // Добавляем блочное устройство
        mBlockDevices.insert(numButton, dev);

        // Добавляем кнопку для блочного устройства
        addToolButton(genInfoAboutBlockDev(dev),
                      ToolButtonIcon,
                      iconSize,
                      numButton,
                      numButton / MaxBlockDevicesInRow,
                      numButton % MaxBlockDevicesInRow);

        // Увеличиваем номер кнопки
        ++numButton;
    }

    // Сброс выбора
    resetSelection();
}

void BlockDevicesSelectionDialog::removeBlockDevices()
{
    mBlockDevices.clear();
    removeButtons();
}

void BlockDevicesSelectionDialog::setInfoFlags(InfoBlockDevices flags)
{
    mInfoBlockDevices = flags;
}

std::shared_ptr<BlockDevice> BlockDevicesSelectionDialog::getSelectedBlockDevice()
{
    const int selectedButtonID = getSelectedId();
    if (selectedButtonID == -1) {
        return {};
    }

    return mBlockDevices.value(selectedButtonID);
}

QString BlockDevicesSelectionDialog::genInfoAboutBlockDev(const std::shared_ptr<BlockDevice> &dev)
{
    if (!dev || !dev->isValid()) {
        return {};
    }

    QString devInfo;

    if (mInfoBlockDevices.testFlag(InfoBlockDevice::UUID)) {
        devInfo.append("UUID: " + dev->getUUID() + '\n');
    }
    if (mInfoBlockDevices.testFlag(InfoBlockDevice::DevName)) {
        devInfo.append(tr("Имя: ") + dev->getDevPath() + '\n');
    }
    if (mInfoBlockDevices.testFlag(InfoBlockDevice::Vendor)) {
        devInfo.append(dev->getVendor() + " ");
    }
    if (mInfoBlockDevices.testFlag(InfoBlockDevice::Model)) {
        devInfo.append(dev->getModel());
    }

    return devInfo;
}

void BlockDevicesSelectionDialog::retranslate()
{
    SelectionDialog::retranslate();
    setInfoText(tr("Выбор флеш-накопителя"));
}
