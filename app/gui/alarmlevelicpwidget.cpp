#include "alarmlevelicpwidget.h"
#include "ui_alarmlevelicpwidget.h"

#include "controller/monitorcontroller.h"
#include "controller/settings.h"
#include "gui/gui_funcs.h"

#include <QDebug>
#include <QSvgWidget>

AlarmLevelICPWidget::AlarmLevelICPWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlarmLevelICPWidget)
{
    ui->setupUi(this);

    // Настройка SVG виджетов
    setupSVGWidgets();
}

AlarmLevelICPWidget::~AlarmLevelICPWidget()
{
    delete ui;
}

void AlarmLevelICPWidget::installController(MonitorController *controller)
{
    if (!controller) {
        return;
    }

    mController = controller;

    connect(mController, &MonitorController::controllerEvent,
            this, &AlarmLevelICPWidget::controllerEventHandler);


//    connect(mController, &MonitorController::dataReadyFromAverageICPController,
//            this, &AlarmLevelICPWidget::updateAverageValueOnWidgets);

    // Обновление информации о тревоге
    updateAlarmInfo();
}

void AlarmLevelICPWidget::scaleFont(float scaleFactor)
{
    WFontScaling(ui->averagePointerWidget, scaleFactor);
}

void AlarmLevelICPWidget::setupSVGWidgets()
{
    if (mLowLevelPressureSVG && mHighLevelPressureSVG) {
        return;
    }

    // Выделяем память
    mLowLevelPressureSVG = new QSvgWidget(this);
    mHighLevelPressureSVG = new QSvgWidget(this);

    // Настраиваем максимальный размер SVG
    mLowLevelPressureSVG->setMaximumSize(MaxAlarmStateSVGSize);
    mHighLevelPressureSVG->setMaximumSize(MaxAlarmStateSVGSize);

    // Располагаем SVG в VLayout
    ui->svgVerticalLayout->addWidget(mHighLevelPressureSVG);
    ui->svgVerticalLayout->addStretch();
    ui->svgVerticalLayout->addWidget(mLowLevelPressureSVG);
}

void AlarmLevelICPWidget::updateAverageValueOnWidgets(double currAverage)//()
{
//    if (!mController) {
//        return;
//    }

//    const ComplexValue val = mController->getLastAverageValue();
//    if(val.valid){
//        ui->averagePointerWidget->setValue(val.value);
//    }
    ui->averagePointerWidget->setValue(currAverage);
}

void AlarmLevelICPWidget::updateAlarmInfo()
{
    updateAlarmLevelsOnWidgets();
    updateAlarmStatesOnWidgets();
}

void AlarmLevelICPWidget::controllerEventHandler(ControllerEvent event)//,const QVariantMap &args)
{
    switch (event) {
    case ControllerEvent::UpdatedAlarmLevels: {
        updateAlarmLevelsOnWidgets();
        break;
    }
    case ControllerEvent::UpdatedAlarmStates: {
        updateAlarmStatesOnWidgets();
        break;
    }
    case ControllerEvent::AlarmLowOn: {
        ui->averagePointerWidget->startBlinkingMode(QColor{55, 185, 235});
        break;
    }
    case ControllerEvent::AlarmOff: {
        ui->averagePointerWidget->stopBlinkingMode();
        break;
    }
    case ControllerEvent::AlarmHighOn: {
        ui->averagePointerWidget->startBlinkingMode(QColor{255, 61, 61});
        break;
    }
    case ControllerEvent::SensorStartDispatchSensorReadings: {
        ui->averagePointerWidget->setPointerVisible(true);
        ui->averagePointerWidget->stopBlinkingMode();
        break;
    }
    case ControllerEvent::SensorStopDispatchSensorReadings: {
        ui->averagePointerWidget->setPointerVisible(false);
        ui->averagePointerWidget->stopBlinkingMode();
        break;
    }
        default: break;
    }
}

void AlarmLevelICPWidget::updateAlarmLevelsOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    ui->averagePointerWidget->setRange(settings->getLowLevelAlarm(), settings->getHighLevelAlarm());
}

void AlarmLevelICPWidget::updateAlarmStatesOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    mLowLevelPressureSVG->load(settings->getLowLevelStateAlarm() ? AlarmEnabledSvg : AlarmDisabledSvg);
    mHighLevelPressureSVG->load(settings->getHighLevelStateAlarm() ? AlarmEnabledSvg : AlarmDisabledSvg);
}

