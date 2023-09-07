#include "systeminfopage.h"
#include "ui_systeminfopage.h"
#include "ui_abstractdialogpage.h"
#include "abstractdialogpage.h"
#include "gui_funcs.h"
#include "version.h"
#include "global_functions.h"
#include "controller/monitorcontroller.h"
#include "blockDevices/blockdevice.h"

#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QStorageInfo>

SystemInfoPage::SystemInfoPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::SystemInfoPage),
    mUpdateMemBarsTimer(new QTimer(this))
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);

    // Меняем изображение для кнопки отменить
    setIconRejectButton(QIcon(":/icons/home.svg"), QIcon(":/icons/home_pressed.svg"));

    // Убираем кнопку подтвердить
    enableAcceptButton(false);

    // Таймер обновления кол-ва доступной памяти
    mUpdateMemBarsTimer->setInterval(IntervalUpdateMemBarsMs);
    connect(mUpdateMemBarsTimer, &QTimer::timeout, this, &SystemInfoPage::updateProgressBars);
}

SystemInfoPage::~SystemInfoPage()
{
    delete ui;
}

void SystemInfoPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Системная информация"));
    setBottomInfoLabel("");
    ui->versionLabel->setText(version::VERSION_STRING2);
}

void SystemInfoPage::softwareStorageUnavailable()
{
    ui->sysMemValueLabel->setText(tr("Не доступно"));
    ui->sysMemPersentFreeSpaceProgressBar->setValue(0);
}

void SystemInfoPage::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
{
    switch (event) {
    case ControllerEvent::SoftwareStorageAvailable:
    case ControllerEvent::SoftwareStorageUnavailable:
    case ControllerEvent::SoftwareStorageNotAssigned:
    {
        mSoftwareStorage = mController->getSoftwareStorage();
        break;
    }
        default: break;
    }
}

void SystemInfoPage::updateProgressBars()
{
    updateRAMProgressBar();
    updateSoftwareStorageProgressBar();
}

void SystemInfoPage::updateRAMProgressBar()
{
    const int64_t FreeRAMMB = freeRAMBytesLinux() / 1024;
    const int64_t TotalRAMMB = totalRAMBytesLinux() / 1024;

    // Отображение доступного кол-ва оперативной памяти пользователю
    if (FreeRAMMB > 0 && TotalRAMMB > 0) {
        ui->ramValueLabel->setText(TemplateMemoryInfo.arg(FreeRAMMB).arg(tr("свободно")).arg(TotalRAMMB));
        ui->ramPersentFreeSpaceProgressBar->setValue(100 - (FreeRAMMB / (double)TotalRAMMB) * 100);
    } else {
        ui->ramValueLabel->setText(tr("Ошибка вычисления"));
        ui->ramPersentFreeSpaceProgressBar->setValue(0);
    }
}

void SystemInfoPage::updateSoftwareStorageProgressBar()
{
    if (!mSoftwareStorage) {
        softwareStorageUnavailable();
        return;
    }

    // Обновляем информацию об устройстве
    mSoftwareStorage->refresh();

    // Если программное хранилище не валидное
    if (!mSoftwareStorage->isValid()) {
        softwareStorageUnavailable();
        return;
    }

    // Информация о программном хранилище
    const auto BlockDevInfo = mSoftwareStorage->getLastStorageInfo();
    if (!BlockDevInfo.isValid()) {
        softwareStorageUnavailable();
        return;
    }

    // Доступное количество памяти в мегабайтах
    const int64_t AvailableMemMB = BlockDevInfo.bytesAvailable() / 1048576;
    const int64_t TotalMemMB = BlockDevInfo.bytesTotal() / 1048576;
    if (AvailableMemMB  <= 0 || TotalMemMB <= 0) {
        softwareStorageUnavailable();
        return;
    }

    // Установка инфо в виджеты
    ui->sysMemValueLabel->setText(TemplateMemoryInfo.arg(AvailableMemMB).arg(tr("доступно")).arg(TotalMemMB));
    ui->sysMemPersentFreeSpaceProgressBar->setValue(100 - (AvailableMemMB / (double)TotalMemMB) * 100);
}

void SystemInfoPage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);
    connect(mController, &MonitorController::controllerEvent, this, &SystemInfoPage::controllerEventHandler);
    retranslate();
}

void SystemInfoPage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);
    WFontScaling(ui->aboutProgramGroupBox, scaleFactor);
    WFontScaling(ui->memoryGroupBox, scaleFactor);
    WFontScaling(ui->ramNameLabel, scaleFactor);
    WFontScaling(ui->ramValueLabel, scaleFactor);
    WFontScaling(ui->sysMemNameLabel, scaleFactor);
    WFontScaling(ui->sysMemValueLabel, scaleFactor);
    WFontScaling(ui->versionProgramGroupBox, scaleFactor);
    WFontScaling(ui->versionLabel, scaleFactor);
    WFontScaling(ui->ramPersentFreeSpaceProgressBar, scaleFactor);
    WFontScaling(ui->sysMemPersentFreeSpaceProgressBar, scaleFactor);
}

void SystemInfoPage::showEvent(QShowEvent */*event*/)
{
    updateProgressBars();
    mUpdateMemBarsTimer->start();
}

void SystemInfoPage::hideEvent(QHideEvent */*event*/)
{
    mUpdateMemBarsTimer->stop();
}
