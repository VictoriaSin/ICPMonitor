#include "averageicpwidget.h"
#include "ui_averageicpwidget.h"
#include "controller/monitorcontroller.h"
#include "sensor/isensor.h"
#include "gui/gui_funcs.h"

#include <math.h>

AverageICPWidget::AverageICPWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AverageICPWidget)
{
    ui->setupUi(this);
    mBufferAverageNumber = new char[mBufferSize];
}

AverageICPWidget::~AverageICPWidget()
{
  delete [] mBufferAverageNumber;
  delete ui;
}

void AverageICPWidget::installController(MonitorController *controller)
{
    mController = controller;

    connect(mController, &MonitorController::controllerEvent,
            this, &AverageICPWidget::controllerEventHandler);

    connect(mController, &MonitorController::dataReadyFromAverageICPController,
                this, &AverageICPWidget::updateAverage);
}

void AverageICPWidget::scaleFont(float scaleFactor)
{
    WFontScaling(ui->ICPLabel, scaleFactor);
    WFontScaling(ui->averageValueLabel, scaleFactor);
    WFontScaling(ui->metricMmHgLabel, scaleFactor);
}

void AverageICPWidget::retranslate()
{
    ui->retranslateUi(this);
}

void AverageICPWidget::updateAverage()
{
    const auto val = mController->getLastAverageValue();
    if(val.valid){
        memset(mBufferAverageNumber, 0, mBufferSize);
        i32toa_countlut((int)std::round(val.value), mBufferAverageNumber);
        ui->averageValueLabel->setText(mBufferAverageNumber);
    }
}

void AverageICPWidget::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
{
    switch (event) {
    case ControllerEvent::SensorStartDispatchSensorReadings: {
        ui->metricMmHgLabel->setVisible(true);
        break;
    }
    case ControllerEvent::SensorStopDispatchSensorReadings: {
        ui->averageValueLabel->setText("--");
        ui->metricMmHgLabel->setVisible(false);
        break;
    }
    case ControllerEvent::SensorConnectionError: {
        ui->averageValueLabel->setText("--");
        ui->metricMmHgLabel->setVisible(false);
        break;
    }
    case ControllerEvent::SensorDisconnected: {
        ui->averageValueLabel->setText("--");
        ui->metricMmHgLabel->setVisible(false);
        break;
    }
    case ControllerEvent::SensorUndefinedError: {
        ui->averageValueLabel->setText("--");
        ui->metricMmHgLabel->setVisible(false);
        break;
    }
    default: break;
    }
}
