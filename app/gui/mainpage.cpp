#include "mainpage.h"
#include "ui_mainpage.h"
#include "controller/monitorcontroller.h"
#include "controller/settings.h"
#include "controller/sessions/session.h"
#include "sensor/isensor.h"
#include "gui/gui_funcs.h"
#include "gui/mainmenu.h"
#include "plots/markitem.h"
#include "plots/labelmarkitem.h"
#include "controller/labels/label.h"

#include <QTimer>
#include <QDateTime>
#include <QPushButton>
#include <QDebug>
#include <QProcess>
#include <QStringList>



QFile mHeadFile;
QFile mIntervalsFile;
QFile mMarksFile;
QFile mRawDataFile;

uint64_t startTimeStampRecord {0};
uint16_t mCurrentLabelIndex;

MainPage::MainPage(QWidget *parent)
    : IPageWidget(parent)
    , mUpdateDateTimeTimer(new QTimer(this))
    , ui(new Ui::MainPage)
{
    ui->setupUi(this);

    // Настройка кнопок
    setupButtons();
    // Настройка окна главного меню
    setupMainMenu();
    // Настройки нижних иконок-уведомлений
    setupBottomInfoSVG();
    // Настройка контейнера с графиками
    setupGraphsContainer();
    connect(this,  &MainPage::resetWaveGraph, mCurrentGraphsArea, &CurrentGraphsArea::resetGraphOfCurrentValues);

    // Для добавления данных на график записи показаний
    connect(this, &MainPage::dataReadyForRecordGraph, mCurrentGraphsArea, &CurrentGraphsArea::addDataOnRecordedPlot);

    // Настраиваем обновление виджета с текущим временем
    mUpdateDateTimeTimer->setInterval(1000);
    connect(mUpdateDateTimeTimer, &QTimer::timeout, this, &MainPage::updateDateTime);

    connect(this, &MainPage::changeCurrentGraph, mCurrentGraphsArea, &AbstractMultipleGraphsAreasWidget::changeGraph);
    connect(this, SIGNAL(changeRecordedGraphInteraction(bool)), mCurrentGraphsArea, SLOT(addOrDeleteNewItem(bool)));

    connect(this, &MainPage::changeCurrentRange, mCurrentGraphsArea, &CurrentGraphsArea::changeXInterval);

    connect(this, SIGNAL(goToLabel(bool)), mCurrentGraphsArea, SLOT(goToLabel(bool)));

}

MainPage::~MainPage()
{
    delete ui;
}

void MainPage::setupButtons()
{
    // Общий стиль для кнопок
    const QString ToolButtonStyleSheet = readStyleSheetFile(":/styles/ToolButtons.qss");

    // Кнопка записи
    ui->recordButton->setIcon(QIcon(":/icons/startRecord.svg"), QIcon(":/icons/startRecord_pressed.svg"));
    ui->recordButton->setIconSize(QSize(100, 100));
    ui->recordButton->setStyleSheet(ToolButtonStyleSheet);
    ui->recordButton->hide();

    // Кнопка установки метки
    ui->makeLabelButton->setIcon(QIcon(":/icons/selectData.svg"), QIcon(":/icons/selectData_pressed.svg"));
    ui->makeLabelButton->setIconSize(QSize(100, 100));
    ui->makeLabelButton->setStyleSheet(ToolButtonStyleSheet);
    ui->makeLabelButton->hide();

    // Кнопка экрана домой
    ui->homeButton->setIcon(QIcon(":/icons/settings.svg"), QIcon(":/icons/settings_pressed.svg"));
    ui->homeButton->setIconSize(QSize(100, 100));
    ui->homeButton->setStyleSheet(ToolButtonStyleSheet);

    // Кнопка начала сессии
    ui->sessionButton->setIcon(QIcon(":/icons/newSession.svg"), QIcon(":/icons/newSession_pressed.svg"));
    ui->sessionButton->setIconSize(QSize(100, 100));
    ui->sessionButton->setStyleSheet(ToolButtonStyleSheet);

    // Кнопка подтверждения метки
    ui->acceptMarkButton->setIcon(QIcon(":/icons/accept.svg"), QIcon(":/icons/accept_pressed.svg"));
    ui->acceptMarkButton->setIconSize(QSize(80, 80));
    ui->acceptMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->acceptMarkButton->hide();

    // Кнопка удаления метки
    ui->rejectMarkButton->setIcon(QIcon(":/icons/reject.svg"), QIcon(":/icons/reject_pressed.svg"));
    ui->rejectMarkButton->setIconSize(QSize(80, 80));
    ui->rejectMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->rejectMarkButton->hide();

    // Кнопки интервалов
    ui->intervalButton->setIcon(QIcon(":/icons/startInterval1.svg"), QIcon(":/icons/startInterval1_pressed.svg"));
    ui->intervalButton->setIconSize(QSize(100, 100));
    ui->intervalButton->setStyleSheet(ToolButtonStyleSheet);
    ui->intervalButton->hide();

    // Кнопка подтверждения интервала
    ui->acceptIntervalButton->setIcon(QIcon(":/icons/accept.svg"), QIcon(":/icons/accept_pressed.svg"));
    ui->acceptIntervalButton->setIconSize(QSize(80, 80));
    ui->acceptIntervalButton->setStyleSheet(ToolButtonStyleSheet);
    ui->acceptIntervalButton->hide();

    // Кнопка удаления интервала
    ui->rejectIntervalButton->setIcon(QIcon(":/icons/reject.svg"), QIcon(":/icons/reject_pressed.svg"));
    ui->rejectIntervalButton->setIconSize(QSize(80, 80));
    ui->rejectIntervalButton->setStyleSheet(ToolButtonStyleSheet);
    ui->rejectIntervalButton->hide();

    // Кнопка перехода к 1 интервалу
    ui->goToInterval1Button->setIcon(QIcon(":/icons/firstInterval.svg"),QIcon(":/icons/firstInterval_pressed.svg"));
    ui->goToInterval1Button->setIconSize(QSize(100, 100));
    ui->goToInterval1Button->setStyleSheet(ToolButtonStyleSheet);
    ui->goToInterval1Button->hide();

    // Кнопка перехода ко 2 интервалу
    ui->goToInterval2Button->setIcon(QIcon(":/icons/secondInterval.svg"),QIcon(":/icons/secondInterval_pressed.svg"));
    ui->goToInterval2Button->setIconSize(QSize(100, 100));
    ui->goToInterval2Button->setStyleSheet(ToolButtonStyleSheet);
    ui->goToInterval2Button->hide();

    ui->goToNextMarkButton->setIcon(QIcon(":/icons/nextLabel.svg"),QIcon(":/icons/nextLabel_pressed.svg"));
    ui->goToNextMarkButton->setIconSize(QSize(100, 100));
    ui->goToNextMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->goToNextMarkButton->show();


    ui->goToPreviousMarkButton->setIcon(QIcon(":/icons/previousLabel.svg"),QIcon(":/icons/previousLabel_pressed.svg"));
    ui->goToPreviousMarkButton->setIconSize(QSize(100, 100));
    ui->goToPreviousMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->goToPreviousMarkButton->show();

    ui->goToPreviousMarkButton->hide();
    ui->goToNextMarkButton->hide();
    ui->labelsNavigation->hide();

    ui->maxValueInterval1->hide();
    ui->maxValueInterval2->hide();
    ui->averageValueInterval1->hide();
    ui->averageValueInterval2->hide();

    ui->alarmLevelICPWidget->hide();
    ui->averageICPWidget->hide();

    ui->sensorStateLabel->hide();
}

void MainPage::setupBottomInfoSVG()
{
    ui->softwareStorageIconSVG->setMaximumSize(48, 60);
}

void MainPage::setupMainMenu()
{
    if (mMainMenu) {
        return;
    }
    mMainMenu = new MainMenu(this);
    // Скрываем основное меню
    mMainMenu->hide();

    // Смена виджета
    connect(mMainMenu, &IPageWidget::previousPage, this, &IPageWidget::previousPage);
    connect(mMainMenu, &IPageWidget::changePage, this, &IPageWidget::changePage);
}

void MainPage::setupGraphsContainer()
{
    // Добавляем графики в контейнер
    mCurrentGraphsArea = ui->currentGraphsArea;
    //qDebug() << "mCurrentGraphsArea = " << mCurrentGraphsArea;
    mGraphWidgetContainer.push_back(mCurrentGraphsArea);
}

void MainPage::installController(MonitorController *controller)
{
    if (!controller) {
        return;
    }

    mController = controller;

    ui->averageICPWidget->installController(mController);
    ui->alarmLevelICPWidget->installController(mController);
    mCurrentGraphsArea->installController(mController);
    mMainMenu->installController(mController);

    connect(mController, &MonitorController::controllerEvent,
            this, &MainPage::controllerEventHandler);

    retranslate();
}

void MainPage::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
{
    switch(event) {
    case ControllerEvent::SessionStarted: {
        //updateSessionInfo(); !!!!
        break;
    }
    case ControllerEvent::SessionClosed: {
        //updateSessionInfo(); !!!!
        break;
    }
    case ControllerEvent::GlobalTimeUpdate: {
        updateDateTime();
        break;
    }
    case ControllerEvent::LabelCreated: {
        updateLabelCounter();
        break;
    }
    case ControllerEvent::SensorConnected: {
        mLastSensorState = ControllerEvent::SensorConnected;
        updateSensorState();
        break;
    }
    case ControllerEvent::SensorConnectionError: {
        mLastSensorState = ControllerEvent::SensorConnectionError;
        updateSensorState();
        break;
    }
    case ControllerEvent::SensorReset: {
        mLastSensorState = ControllerEvent::SensorReset;
        updateSensorState();
        break;
    }
    case ControllerEvent::SensorResetError: {
        mLastSensorState = ControllerEvent::SensorResetError;
        updateSensorState();
        break;
    }
    case ControllerEvent::SensorDisconnected: {
        mLastSensorState = ControllerEvent::SensorDisconnected;
        updateSensorState();
        break;
    }
    case ControllerEvent::SoftwareStorageAvailable: {
        updateSoftwareStorageOnWidgets(true);
        break;
    }
    case ControllerEvent::SoftwareStorageUnavailable: {
        updateSoftwareStorageOnWidgets(false);
        break;
    }
    case ControllerEvent::SoftwareStorageNotAssigned: {
        updateSoftwareStorageOnWidgets(false);
        break;
    }
    case ControllerEvent::DataBaseAvailable: {
//!!!   ui->makeLabelButton->setDisabled(false);
        ui->softwareStorageIconSVG->load(QString(":/icons/softwareStorage.svg"));
        break;
    }
    case ControllerEvent::DataBaseUnavailable: {
//!!!   ui->makeLabelButton->setDisabled(true);
        ui->softwareStorageIconSVG->load(QString(":/icons/softwareStorageUnavailable.svg"));
        break;
    }
//    case ControllerEvent::ScreenWritten:
//    case ControllerEvent::ScreenNotWritten: {
//        ui->photoButton->setEnabled(true);
//        break;
//    }
    default: break;
    }
}

void MainPage::updateSensorInfo()
{
    // Если контроллер не установлен
    if (!mController) {
        return;
    }

    // Получаем текущий датчик
    const auto sensor = mController->sensor();
    if (!sensor) {
        return;
    }

//    // Устанавливаем информацию о датчике
//    ui->sensorInfoLabel->setText(sensor->info());
}

void MainPage::updateSensorResetTime()
{
    // Если контроллер не установлен
    if (!mController) {
        return;
    }

    // Получаем текущий датчик
    const ISensor * const sensor = mController->sensor();
    if (!sensor) {
        return;
    }

    // Обновляем информацию
    ui->sensorStateLabel->setText(QString("%1: %2").arg(tr("Обнулён")).
                                      arg(QDateTime::fromMSecsSinceEpoch(sensor->lastResetTimestamp()).toString("hh:mm:ss-dd.MM.yyyy")));
}

void MainPage::updateSensorState()
{
    switch (mLastSensorState) {
    case ControllerEvent::SensorConnected: {
        ui->sensorStateLabel->setText(tr("Датчик подключён"));
        updateSensorInfo();
        break;
    }
    case ControllerEvent::SensorConnectionError: {
        ui->sensorStateLabel->setText(tr("Ошибка соединения с датчиком"));
        break;
    }
    case ControllerEvent::SensorReset: {
        updateSensorResetTime();
        break;
    }
    case ControllerEvent::SensorResetError: {
        ui->sensorStateLabel->setText(tr("Ошибка сброса датчика"));
        break;
    }
    case ControllerEvent::SensorDisconnected: {
        ui->sensorStateLabel->setText(tr("Датчик отключён"));
        updateSensorInfo();
        break;
    }
        default: break;
    }
}

void MainPage::updateSessionInfo()
{
    // Если контроллер не установлен
    if (!mController) {
        return;
    }

    // Получаем текущую сессию
    const auto &currentSession = mController->getCurrentSession();
//    if (!currentSession) {
//        ui->sessionID->setText(tr("Сессия неактивна!"));
//    } else {
//        ui->sessionID->setText(QString("%1 %2").arg(tr("ID сессии:")).arg(currentSession->getId()));
//    }
}

void MainPage::updateLabelCounter()
{
    // Если контроллер не установлен
    if (!mController) {
        return;
    }

    // Подготовка шаблона для заполнения
    const QString counterLabelText = tr("Меток: %1");
    const QString currentLabelText = tr("%1/%2");
    // Запрос у контроллера кол-ва меток за текущую сессию
    int countLabels = mController->getLabelsCountPerCurrentSession();
    if (countLabels > 0) {
        //ui->labelCounterLabel->setText(counterLabelText.arg(mLabelManagerGlobal->mCountLabels));
        ui->labelsNavigation->setText(currentLabelText.arg(mCurrentLabelIndex + 1).arg(mLabelManagerGlobal->mCountLabels));
    } else {
        //ui->labelCounterLabel->setText(tr("Меток: "));
    }
}

void MainPage::scaleFont(float scaleFactor)
{
    WFontScaling(ui->dateTimeLabel, scaleFactor);
    //WFontScaling(ui->sensorInfoLabel, scaleFactor);
    WFontScaling(ui->sensorStateLabel, scaleFactor);
    //WFontScaling(ui->labelCounterLabel, scaleFactor);
    //WFontScaling(ui->sessionID, scaleFactor);
    ui->averageICPWidget->scaleFont(scaleFactor);
    ui->alarmLevelICPWidget->scaleFont(scaleFactor);
    mCurrentGraphsArea->scaleFont(scaleFactor);
    mMainMenu->scaleFont(scaleFactor);
    WFontScaling(ui->labelsNavigation, scaleFactor);
    WFontScaling(ui->maxValueInterval1, scaleFactor);
    WFontScaling(ui->maxValueInterval2, scaleFactor);
    WFontScaling(ui->averageValueInterval1, scaleFactor);
    WFontScaling(ui->averageValueInterval2, scaleFactor);
}

void MainPage::updateDateTime()
{
    auto currentDateTime = QDateTime::currentDateTime().toString("hh:mm\ndd.MM.yyyy");
    if (currentDateTime != mLastDateTime) {
        ui->dateTimeLabel->setText(currentDateTime);
        mLastDateTime = currentDateTime;

        if (mController) {
            QTimer::singleShot(0, mController, [this](){
                mController->saveCurrentDateTime();
            });
        }
    }
}

void MainPage::updateSoftwareStorageOnWidgets(bool isAvailable)
{
    QString svgPath;
    if (isAvailable) {
        svgPath = ":/icons/softwareStorage.svg";
    } else {
        svgPath = ":/icons/softwareStorageUnavailable.svg";
    }

    ui->softwareStorageIconSVG->load(svgPath);
}

void MainPage::makeScreen()
{
    if (!mController) {
        return;
    }

//#ifdef PC_BUILD
//    const QPixmap &&screen(makeAScreenOfWidget(this));
//    if (!screen.isNull()) {
//        ui->photoButton->setDisabled(true);
//        QTimer::singleShot(0, mController, [this, scr = std::move(screen)](){
//            mController->writeScreenFromWidget(scr);
//        });
//    }
//#else
//    QTimer::singleShot(0, mController, [this](){
//        ui->photoButton->setDisabled(true);
//        mController->writeScreenFromLinuxFB();
//    });
//#endif
}

void MainPage::nextGraph()
{
    // Кол-во графиков
    int countGraph = mGraphWidgetContainer.size();

    // Если в списке меньше одного графика
    if (countGraph < 2) {
        return;
    }

    // Скрываем текущий график
    mGraphWidgetContainer[mGraphNumber]->hide();

    // Показываем следующий график
    ++mGraphNumber;
    mGraphNumber %= countGraph;
    mGraphWidgetContainer[mGraphNumber]->show();
}

void MainPage::showEvent(QShowEvent *event)
{
    updateDateTime();
    mUpdateDateTimeTimer->start();
}

void MainPage::hideEvent(QHideEvent *event)
{
    mUpdateDateTimeTimer->stop();
}

void MainPage::retranslate()
{
    ui->averageICPWidget->retranslate();
    mCurrentGraphsArea->retranslate();
    //updateSessionInfo(); !!!!
    updateSensorState();
    updateSensorInfo();
    updateLabelCounter();
    mMainMenu->retranslate();
}



void MainPage::on_homeButton_clicked()
{
    emit changePage(mMainMenu);
}


void MainPage::on_recordButton_clicked()
{
        QString currentTime = QDateTime::currentDateTime().toString("yyyy_MM_dd@hh_mm_ss");
        if (isStart)
        {
            qDebug() << currentTime;
            isStart = false;
            ui->recordButton->setIcon(QIcon(":/icons/stopRecord.svg"), QIcon(":/icons/stopRecord_pressed.svg"));
            emit (resetWaveGraph());
    #ifdef PC_BUILD
            QDir mCurrentRecordDir(mController->getSoftwareStorage()->mInfo.rootPath() + "/" + currentTime);
    #else
            QDir mCurrentRecordDir("/media/ICPMonitor_AllRecordedData/" + currentTime);
    #endif


            mHeadFile.setFileName(mCurrentRecordDir.path() + "/" + "HEAD.txt");
            mIntervalsFile.setFileName(mCurrentRecordDir.path() + "/" + "INTERVALS.txt");
            mMarksFile.setFileName(mCurrentRecordDir.path() + "/" + "MARKS.txt");
            mRawDataFile.setFileName(mCurrentRecordDir.path() + "/" + "RAW_DATA.txt");


            qDebug() << mCurrentRecordDir.path();
            if (!mCurrentRecordDir.exists())
            {
                //qDebug() << "not exists";
#ifdef TEST_BUILD
                QProcess process;
                process.start("mkdir", QStringList() << "-m" << "777" << mCurrentRecordDir.path());
                process.waitForFinished();
                //qDebug() << "normal = "<< process.readAllStandardOutput();
                //qDebug() << "error =" << process.readAllStandardError();
                process.close();
#elif PC_BUILD
                mCurrentRecordDir.mkdir(mCurrentRecordDir.path());
#endif

            }
            mCurrentGraphsArea->isRecord = true;
            qDebug() << "isRecord" << mCurrentGraphsArea->isRecord;
            startTimeStampRecord = getCurrentTimeStampMS();
            qDebug() << startTimeStampRecord;
            if (mHeadFile.open(QIODevice::WriteOnly | QIODevice::Append))
            {
                QStringList time = currentTime.split("@");
                mHeadFile.write(("Session started: " + time[0].replace("_", ".") + " " + time[1].replace("_", ":") +
                        "\nStartTimeStamp(ms): " + QString::number(startTimeStampRecord) + "\n").toLatin1());
                mHeadFile.close();
            }
            ui->mainWidgets->show();
            //ui->softwareStorageIconSVG->show();
            //ui->dateTimeLabel->show();
            //ui->infoWidgets->show();
            ui->sessionButton->setEnabled(false);
        }
        else
        {
            uint64_t StopMS = getCurrentTimeStampMS();
            currentTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
            qDebug() << "Stop" << currentTime;
            if (mHeadFile.open(QIODevice::WriteOnly | QIODevice::Append))
            {
                //QTextStream data(&mHeadFile);
                QStringList time = currentTime.split("@");
                //data << "Session stopped: " + currentTime + "\nStopTimeStamp(ms): " << StopMS << "\n";
                mHeadFile.write(("Session stopped: " + currentTime + "\nStopTimeStamp(ms): " + QString::number(StopMS) + "\n").toLatin1());
                mHeadFile.close();
            }
            isStart = true;
            ui->recordButton->setIcon(QIcon(":/icons/startRecord.svg"), QIcon(":/icons/startRecord_pressed.svg"));
            mCurrentGraphsArea->isRecord = false;
            emit(dataReadyForRecordGraph());
            //mController->stopAverageTimer();// получается не нужен
            mController->mSensor->endSendingSensorReadings();

            //ui->softwareStorageIconSVG->hide();
            //ui->dateTimeLabel->hide();
            //ui->infoWidgets->hide();
            ui->intervalButton->show();
            //ui->intervalStopButton->show();
            ui->makeLabelButton->show();
            ui->recordButton->hide();
            ui->sessionButton->setEnabled(true);

            ui->averageICPWidget->hide();
            ui->alarmLevelICPWidget->hide();
            emit (changeCurrentGraph());
        }
}


void MainPage::on_makeLabelButton_clicked()
{
    ui->makeLabelButton->setEnabled(false);
    ui->acceptMarkButton->show();
    ui->rejectMarkButton->show();
    QTimer::singleShot(0, mController, [this](){
        mController->makeLabel();
    });
    ui->intervalButton->hide();
    //emit (changeLabelButtonStatus);
}

void MainPage::on_acceptMarkButton_clicked()
{
    ui->labelsNavigation->show();
    ui->goToPreviousMarkButton->show();
    ui->goToNextMarkButton->show();
    ui->makeLabelButton->setEnabled(true);
    ui->acceptMarkButton->hide();
    ui->rejectMarkButton->hide();
    mMarksFile.open(QIODevice::WriteOnly | QIODevice::Append);
    mMarksFile.write((QString::number(mLabelManagerGlobal->mCountLabels) + ": " + QString::number(mCoordLabelX) + "\n").toLatin1());
    mMarksFile.close();
    Label *temp = mLabelItemsContainer.back()->getLabel();
    temp->mCurrentPos = (double)mCoordLabelX/1000;
    if (mIntervalsCount < 4) {ui->intervalButton->show();}
    emit(changeRecordedGraphInteraction(true));
}


void MainPage::on_rejectMarkButton_clicked()
{
    ui->makeLabelButton->setEnabled(true);
    ui->acceptMarkButton->hide();
    ui->rejectMarkButton->hide();
    if (mIntervalsCount < 4) {ui->intervalButton->show();}
    emit(changeRecordedGraphInteraction(false));
    updateLabelCounter();
}


void MainPage::on_sessionButton_clicked()
{
    if (isSessionStart)
    {
        isSessionStart = false;
        ui->recordButton->show();
        ui->recordButton->setEnabled(true);
        ui->sessionButton->setIcon(QIcon(":/icons/deleteSession.svg"), QIcon(":/icons/deleteSession_pressed.svg"));
        ui->homeButton->hide();
        ui->makeLabelButton->hide();
        ui->acceptMarkButton->hide();
        ui->rejectMarkButton->hide();
        ui->averageICPWidget->show();
        ui->alarmLevelICPWidget->show();

        mCurrentLabelIndex = 0;
        mController->mSensor->startSendingSensorReadings();
        mCurrentGraphsArea->startPlotting();
    }
    else
    {
        isSessionStart = true;
        ui->sessionButton->setIcon(QIcon(":/icons/newSession.svg"), QIcon(":/icons/newSession_pressed.svg"));
        mCurrentGraphsArea->stopPlotting();
        mController->mSensor->endSendingSensorReadings();
        mController->closeSession();

        emit (changeCurrentGraph());
        if (!ui->recordButton->isHidden())
        {
            emit (changeCurrentGraph());
        }
        emit (resetWaveGraph());

        ui->makeLabelButton->hide();
        ui->acceptMarkButton->hide();
        ui->rejectMarkButton->hide();
        ui->recordButton->hide();

        ui->intervalButton->hide();
        ui->acceptIntervalButton->hide();
        ui->rejectIntervalButton->hide();

        ui->homeButton->show();
        ui->goToInterval1Button->hide();
        ui->goToInterval2Button->hide();

        ui->labelsNavigation->hide();
        ui->goToPreviousMarkButton->hide();
        ui->goToNextMarkButton->hide();

        ui->maxValueInterval1->hide();
        ui->maxValueInterval2->hide();
        ui->averageValueInterval1->hide();
        ui->averageValueInterval2->hide();
        //добавить удаление всех линий после завершения сессии

        ui->alarmLevelICPWidget->hide();
        ui->averageICPWidget->hide();
    }
}


void MainPage::on_intervalButton_clicked()
{
    ui->acceptIntervalButton->show();
    ui->rejectIntervalButton->show();

    ui->makeLabelButton->hide();

    mCurrentGraphsArea->addIntervalOnRecordedGraph();
    if (isIntervalCreating)
    {
        ui->intervalButton->setEnabled(false);
    }
}


void MainPage::on_acceptIntervalButton_clicked()
{
    if (isStartInterval)
    {
        if (mIntervalsCount == 1)
        {
            ui->intervalButton->setIcon(QIcon(":/icons/stopInterval1.svg"), QIcon(":/icons/stopInterval1_pressed.svg"));
        }
        else
        {
            ui->intervalButton->setIcon(QIcon(":/icons/stopInterval2.svg"), QIcon(":/icons/stopInterval2_pressed.svg"));
        }
        isStartInterval = false;
    }
    else
    {
        ui->intervalButton->setIcon(QIcon(":/icons/startInterval2.svg"), QIcon(":/icons/startInterval2_pressed.svg"));
        isStartInterval = true;
    }
    mIntervalsFile.open(QIODevice::WriteOnly | QIODevice::Append);
    mIntervalsFile.write((QString::number(mIntervalsCount) + ": " + QString::number(mIntervalsContainer[mIntervalsCount-1]->mIntervalPos) + "\n").toLatin1());
    mIntervalsFile.close();
    ui->makeLabelButton->show();
    ui->acceptIntervalButton->hide();
    ui->rejectIntervalButton->hide();
    emit(changeRecordedGraphInteraction(true));

    ui->intervalButton->setEnabled(true);
    const QString maxValuePreset = tr("Максимум\n%1"); //перевести потом
    const QString averageValuePreset = tr("Среднее\n%1"); //перевести потом
    if (mIntervalsCount % 2 == 0)
    {
        mCurrentGraphsArea->colorInterval();
        if (mIntervalsCount == 2)
        {
            ui->goToInterval1Button->show();
            ui->maxValueInterval1->show();
            ui->averageValueInterval1->show();
            ui->maxValueInterval1->setText(maxValuePreset.arg(QString::number(mIntervalsContainer[mIntervalsCount-1]->maxIntervalValue)));
            ui->averageValueInterval1->setText(averageValuePreset.arg(QString::number(mIntervalsContainer[mIntervalsCount-1]->averageIntervalValue)));
        }
        else
        {
            ui->goToInterval2Button->show();
            ui->maxValueInterval2->show();
            ui->averageValueInterval2->show();
            ui->maxValueInterval2->setText(maxValuePreset.arg(QString::number(mIntervalsContainer[mIntervalsCount-1]->maxIntervalValue)));
            ui->averageValueInterval2->setText(averageValuePreset.arg(QString::number(mIntervalsContainer[mIntervalsCount-1]->averageIntervalValue)));

        }

    }

    if (mIntervalsCount == 4)
    {
        ui->intervalButton->hide();
    }


}


void MainPage::on_rejectIntervalButton_clicked()
{
    ui->acceptIntervalButton->hide();
    ui->rejectIntervalButton->hide();

    ui->makeLabelButton->show();
    mIntervalsCount--;
    emit(changeRecordedGraphInteraction(false));
    ui->intervalButton->setEnabled(true);
}


void MainPage::on_goToInterval1Button_clicked()
{
    emit (changeCurrentRange(first));
}

void MainPage::on_goToInterval2Button_clicked()
{
    emit (changeCurrentRange(second));
}

void MainPage::on_goToPreviousMarkButton_clicked()
{
    emit (goToLabel(previous));
    updateLabelCounter();
}


void MainPage::on_goToNextMarkButton_clicked()
{
    emit (goToLabel(next));
    updateLabelCounter();
}

