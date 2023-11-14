#include "technicalaccesspage.h"
#include "ui_technicalaccesspage.h"
#include "ui_abstractdialogpage.h"
#include "gui/dialogWindows/blockdevicesselectiondialog.h"
#include "gui/gui_funcs.h"
#include "controller/monitorcontroller.h"
#include "controller/settings.h"

#include <QTimer>
#include <QSpinBox>

TechnicalAccessPage::TechnicalAccessPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::TechnicalAccessPage),
    mSetSoftwareStorageDialog(new BlockDevicesSelectionDialog(this))
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);

    // Меняем изображение для кнопки отменить
    setIconRejectButton(BackButtonIcon, BackButtonIconPressed);

    // Убираем кнопку подтвердить
    enableAcceptButton(false);

    // Инициализация спинбокса для максимального кол-ва скринов
    setupMaxScreensSpinBox();

    // Установка отображаемых данных о блочных устройствах
    mSetSoftwareStorageDialog->setInfoFlags(BlockDevicesSelectionDialog::UUID |
                                            BlockDevicesSelectionDialog::DevName |
                                            BlockDevicesSelectionDialog::Vendor |
                                            BlockDevicesSelectionDialog::Model);

    connect(ui->selectSoftwareStorageTButton, &QToolButton::clicked, this, &TechnicalAccessPage::openSoftwareStorageDialog);
    connect(mSetSoftwareStorageDialog, &BlockDevicesSelectionDialog::finished, this, &TechnicalAccessPage::closedSoftwareStorageDialog);

}

TechnicalAccessPage::~TechnicalAccessPage()
{
    delete ui;
}

void TechnicalAccessPage::setupMaxScreensSpinBox()
{
    if (ui->maxScreensSpinBox->currentSpinBox()) {
        return;
    }

    auto maxScreensSpinBox = new QSpinBox(ui->maxScreensSpinBox);
    maxScreensSpinBox->setMinimumWidth(100);
    ui->maxScreensSpinBox->setSpinBox(maxScreensSpinBox);
    ui->maxScreensSpinBox->setRange(0, 1000);
}

void TechnicalAccessPage::updateSelectSoftwareStorageDialog()
{
    if (!mController || !mSetSoftwareStorageDialog) {
        return;
    }

    mSetSoftwareStorageDialog->setBlockDevices(mController->getAvailableBlockDevices());
}

void TechnicalAccessPage::updateCurrentSoftwareStorageLabel()
{
    if (!mController) {
        return;
    }

    // Получаем настройки приложения
    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    // Устанавливаем состояние программного хранилища
    QString softwareStorageState;
    switch (mController->getSoftwareStorageState()) {
    case ControllerEvent::SoftwareStorageAvailable: {
        softwareStorageState = tr("Доступно");
        break;
    }
    case ControllerEvent::SoftwareStorageUnavailable: {
        softwareStorageState = tr("Недоступно");
        break;
    }
    case ControllerEvent::SoftwareStorageNotAssigned: {
        softwareStorageState = tr("Не назначено");
        break;
    }
    default: break;
    }

    // Устанавливаем состояние и текущий UUID программного хранилища
    const QString UUIDSoftwareStorage = settings->getSoftwareStorageUUID();
    if (UUIDSoftwareStorage.isEmpty()) {
        ui->UUIDSoftwareStorageLabel->setText(softwareStorageState);
    } else {
        ui->UUIDSoftwareStorageLabel->setText('(' + softwareStorageState + ") " + UUIDSoftwareStorage);
    }
}

void TechnicalAccessPage::updateMaxScreensLabel()
{
    if (!mController) {
        return;
    }

    // Получаем настройки приложения
    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    //ui->maxScreensSpinBox->setValue(mController->settings()->getMaxScreens());
}

void TechnicalAccessPage::updateMaxScreens()
{
//    if (!mController) {
//        return;
//    }

//    bool isGood {false};
//    const int maxScreens = ui->maxScreensSpinBox->value(&isGood);
//    if (isGood) {QTimer::singleShot(0, mController, [this, maxScreens](){mController->setMaxScreens(maxScreens);});
//    }
}

void TechnicalAccessPage::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
{
    switch (event) {
    case ControllerEvent::BlockDeviceConnected: {
        if (!mSetSoftwareStorageDialog->isHidden()) {
            updateSelectSoftwareStorageDialog();
            mSetSoftwareStorageDialog->setCenter();
        }
        break;
    }
    case ControllerEvent::BlockDeviceDisconnected: {
        if (!mSetSoftwareStorageDialog->isHidden()) {
            updateSelectSoftwareStorageDialog();
            mSetSoftwareStorageDialog->setCenter();
        }
        break;
    }
    case ControllerEvent::SoftwareStorageAvailable:
    case ControllerEvent::SoftwareStorageUnavailable:
    case ControllerEvent::SoftwareStorageNotAssigned: {
        updateCurrentSoftwareStorageLabel();
        break;
    }
    case ControllerEvent::UpdatedMaxScreens: {
        updateMaxScreensLabel();
        break;
    }
        default: break;
    }
}

void TechnicalAccessPage::openSoftwareStorageDialog()
{
    updateSelectSoftwareStorageDialog();
    mSetSoftwareStorageDialog->setCenter();
    mSetSoftwareStorageDialog->open();
}

void TechnicalAccessPage::closedSoftwareStorageDialog(int result)
{
    if (!mController || !mSetSoftwareStorageDialog) {
        return;
    }

    if (result != QDialog::Accepted) {
        return;
    }

    // Установка нового программного хранилища
    QTimer::singleShot(0, mController, [this](){
        mController->setSoftwareStorage(mSetSoftwareStorageDialog->getSelectedBlockDevice());
    });
}

void TechnicalAccessPage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);

    WFontScaling(ui->selectSoftwareStorageTButton, scaleFactor);
    WFontScaling(ui->SoftwareStorageLabel, scaleFactor);
    WFontScaling(ui->UUIDSoftwareStorageLabel, scaleFactor);
    WFontScaling(ui->maxScreensLabel, scaleFactor);
    WFontScaling(ui->maxScreensSpinBox, scaleFactor);

    mSetSoftwareStorageDialog->scaleFont(scaleFactor);
}

void TechnicalAccessPage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);
    connect(controller, &MonitorController::controllerEvent, this, &TechnicalAccessPage::controllerEventHandler);
    retranslate();
}

void TechnicalAccessPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Технический доступ"));
    setBottomInfoLabel(tr("Глубокая настройка устройства"));
    updateCurrentSoftwareStorageLabel();

    mSetSoftwareStorageDialog->retranslate();
}

void TechnicalAccessPage::showEvent(QShowEvent *event)
{
    updateCurrentSoftwareStorageLabel();
    updateMaxScreensLabel();
    AbstractDialogPage::showEvent(event);
}

void TechnicalAccessPage::done(int /*exodus*/)
{
    updateMaxScreens();
    emit previousPage();
}
