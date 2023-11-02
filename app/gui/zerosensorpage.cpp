#include "zerosensorpage.h"
#include "ui_zerosensorpage.h"
#include "ui_abstractdialogpage.h"
#include "controller/monitorcontroller.h"
#include "gui/gui_funcs.h"
#include "sensor/isensor.h"

#include <QTimer>

ZeroSensorPage::ZeroSensorPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::ZeroSensorPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);
    // Настройка кнопки сброса датчика
    setupResetSensorToolButton();
    // Меняем изображение для кнопки отменить
    setIconRejectButton(QIcon(":/icons/goBack.svg"), QIcon(":/icons/goBack_pressed.svg"));
    // Убираем кнопку подтвердить
    enableAcceptButton(false);
#ifdef PC_BUILD
    ui->firstStepLabel->setFont(QFont("Nano Sans",17));
    ui->secondStepLabel->setFont(QFont("Nano Sans",17));
    ui->readyOrNotLabel->setFont(QFont("Nano Sans",17));
#endif
}

ZeroSensorPage::~ZeroSensorPage()
{
    delete ui;
}

void ZeroSensorPage::setupResetSensorToolButton()
{
    ui->resetSensorButton->setIcon(QIcon(":/icons/zeroReset.svg"), QIcon(":/icons/zeroReset_pressed.svg"));
    ui->resetSensorButton->setIconSize(QSize(200, 200));
    ui->resetSensorButton->setStyleSheet(readStyleSheetFile(":/styles/ToolButtons.qss"));
    connect(ui->resetSensorButton, &QToolButton::clicked, this, &ZeroSensorPage::testResetSensor);
}

void ZeroSensorPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);
    setUpperNamePageLabel(tr("Обнуление датчика"));
    updateInfo();
}

void ZeroSensorPage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);

    WFontScaling(ui->firstStepLabel, scaleFactor);
    WFontScaling(ui->secondStepLabel, scaleFactor);
    WFontScaling(ui->readyOrNotLabel, scaleFactor);
}

void ZeroSensorPage::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
{
    switch(event) {
    case ControllerEvent::SensorConnectionError: {
        mLastSensorState = ControllerEvent::SensorConnectionError;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorConfigError: {
        mLastSensorState = ControllerEvent::SensorConfigError;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorDisconnected: {
        mLastSensorState = ControllerEvent::SensorDisconnected;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorReset: {
        mLastSensorState = ControllerEvent::SensorReset;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorResetError: {
        mLastSensorState = ControllerEvent::SensorResetError;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorUndefinedError: {
        mLastSensorState = ControllerEvent::SensorUndefinedError;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorInited: {
        mLastSensorState = ControllerEvent::SensorInited;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorInitError: {
        mLastSensorState = ControllerEvent::SensorInitError;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorConnected: {
        mLastSensorState = ControllerEvent::SensorConnected;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorConfigured: {
        mLastSensorState = ControllerEvent::SensorConfigured;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorStartDispatchSensorReadings: {
        mLastSensorState = ControllerEvent::SensorStartDispatchSensorReadings;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorStopDispatchSensorReadings: {
        mLastSensorState = ControllerEvent::SensorStopDispatchSensorReadings;
        updateInfo();
        break;
    }
    case ControllerEvent::SensorHardReseted: {
        mLastSensorState = ControllerEvent::SensorHardReseted;
        updateInfo();
        break;
    }
        default: break;
    }
}

void ZeroSensorPage::updateInfo()
{
    // Установка информации внизу страницы
    switch (mLastSensorState) {
    case ControllerEvent::SensorConnectionError:
    {
        AbstractDialogPage::ui->infoLabel->setText(tr("Ошибка подключения к датчику"));
        break;
    }
    case ControllerEvent::SensorConfigError: {
        AbstractDialogPage::ui->infoLabel->setText(tr("Ошибка конфигурации датчика"));
        break;
    }
    case ControllerEvent::SensorDisconnected: {
        AbstractDialogPage::ui->infoLabel->setText(tr("Датчик отключён"));
        break;
    }
    case ControllerEvent::SensorReset: {
        AbstractDialogPage::ui->infoLabel->setText(tr("Датчик обнулён"));
        break;
    }
    case ControllerEvent::SensorResetError: {
        AbstractDialogPage::ui->infoLabel->setText(tr("Ошибка обнуления датчика"));
        break;
    }
    case ControllerEvent::SensorUndefinedError: {
        AbstractDialogPage::ui->infoLabel->setText(tr("Неопределенная ошибка датчика"));
        break;
    }
    case ControllerEvent::SensorInited: {
        AbstractDialogPage::ui->infoLabel->setText("");
        break;
    }
    case ControllerEvent::SensorInitError: {
        AbstractDialogPage::ui->infoLabel->setText("");
        break;
    }
    case ControllerEvent::SensorConnected: {
        AbstractDialogPage::ui->infoLabel->setText("");
        break;
    }
    case ControllerEvent::SensorConfigured: {
        AbstractDialogPage::ui->infoLabel->setText("");
        break;
    }
    case ControllerEvent::SensorStartDispatchSensorReadings: {
        break;
    }
    case ControllerEvent::SensorStopDispatchSensorReadings: {
        break;
    }
    case ControllerEvent::SensorHardReseted: {
        AbstractDialogPage::ui->infoLabel->setText("");
        break;
    }
        default: break;
    }

    if (!mController) {
        return;
    }

    //const ISensor * const sensor = mController->sensor();
    //if (!sensor) {
    //    AbstractDialogPage::ui->infoLabel->setText("");
    //    ui->readyOrNotLabel->setText(tr("Датчик не готов к обнулению"));
    //    ui->resetSensorButton->setEnabled(false);
    //    return;
    //}

    // Если датчик может быть обнулён и нуждается в этом
    //if (sensor->canMakeSensorReset() && sensor->needMakeSensorReset()) {
        ui->readyOrNotLabel->setText(tr("Датчик готов к обнулению"));
        ui->resetSensorButton->setEnabled(true);
    //} else
    //{
    //    ui->readyOrNotLabel->setText(tr("Датчик не готов к обнулению"));
    //    ui->resetSensorButton->setEnabled(false);
    //}
}

void ZeroSensorPage::testResetSensor()
{
    if (!mController) { return;}
#ifdef PC_BUILD
    //QTimer::singleShot(0, mController, [this](){mController->testMakeSensorReset();});
    //QTimer::singleShot(0, MainWindow, [this]() { mController->s(QDateTime(da, ti).toSecsSinceEpoch()); });
#endif
    //QTimer::singleShot(0, mController, [this](){mController->testMakeSensorReset();});
    emit previousPage();
}

void ZeroSensorPage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);
    connect(mController, &MonitorController::controllerEvent, this, &ZeroSensorPage::controllerEventHandler);
    retranslate();
}

void ZeroSensorPage::showEvent(QShowEvent */*event*/)
{
    updateInfo();
}
