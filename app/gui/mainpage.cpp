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
#include "global_functions.h"




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

bool isPlayRecord = true;
int currSpeed = speed::SpeedX1;

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
    mUpdateDateTimeTimer->setInterval(60000);
    connect(mUpdateDateTimeTimer, &QTimer::timeout, this, &MainPage::updateDateTime);

    connect(this, &MainPage::changeCurrentGraph, mCurrentGraphsArea, &AbstractMultipleGraphsAreasWidget::changeGraph);
    connect(this, SIGNAL(changeRecordedGraphInteraction(bool)), mCurrentGraphsArea, SLOT(addOrDeleteNewItem(bool)));

    connect(this, &MainPage::changeCurrentRange, mCurrentGraphsArea, &CurrentGraphsArea::changeXInterval);

    connect(this, SIGNAL(goToLabel(bool)), mCurrentGraphsArea, SLOT(goToLabel(bool)));

    connect(this, &MainPage::playBtnPressed, [this](){mCurrentGraphsArea->mRecordedGraph->animateGraphic(currSpeed);});//&RecordedPlot::animateGraphic);

    connect(mCurrentGraphsArea->mRecordedGraph, &RecordedPlot::changeBtnIcon,
            [this]() {ui->playRecord->setIcon(QIcon(":/icons/playRecord.svg"),QIcon(":/icons/playRecord_pressed.svg"));
                      ui->intervalButton->setEnabled(true);
                      ui->goToInterval1Button->setEnabled(true);
                      ui->goToInterval2Button->setEnabled(true);
                      ui->goToNextMarkButton->setEnabled(true);
                      ui->goToPreviousMarkButton->setEnabled(true);
                      ui->makeLabelButton->setEnabled(true);
                      ui->sessionButton->setEnabled(true);
                      ui->rewindRecordButton->setEnabled(true);
                      ui->speedRecordButton->setEnabled(true);
                      ui->downloadGraphButton->setEnabled(true);// временно
                    } );


    ui->averageValue->hide();
}

MainPage::~MainPage()
{
    delete ui;
}
#define BUT_SIZE_BIG 85
#define BUT_SIZE_SMALL 85
void MainPage::setupButtons()
{
    // Общий стиль для кнопок
    const QString ToolButtonStyleSheet = readStyleSheetFile(":/styles/ToolButtons.qss");

    // Кнопка записи
    ui->recordButton->setIcon(QIcon(":/icons/startRecord.svg"), QIcon(":/icons/startRecord_pressed.svg"));
    ui->recordButton->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->recordButton->setStyleSheet(ToolButtonStyleSheet);
    ui->recordButton->hide();

    // Кнопка установки метки
    ui->makeLabelButton->setIcon(QIcon(":/icons/selectData.svg"), QIcon(":/icons/selectData_pressed.svg"));
    ui->makeLabelButton->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->makeLabelButton->setStyleSheet(ToolButtonStyleSheet);
    ui->makeLabelButton->hide();

    // Кнопка экрана домой
    ui->homeButton->setIcon(QIcon(":/icons/settings.svg"), QIcon(":/icons/settings_pressed.svg"));
    ui->homeButton->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->homeButton->setStyleSheet(ToolButtonStyleSheet);

    // Кнопка начала сессии
    ui->sessionButton->setIcon(QIcon(":/icons/newSession.svg"), QIcon(":/icons/newSession_pressed.svg"));
    ui->sessionButton->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->sessionButton->setStyleSheet(ToolButtonStyleSheet);

    // Кнопка подтверждения метки
    ui->acceptMarkButton->setIcon(QIcon(":/icons/accept.svg"), QIcon(":/icons/accept_pressed.svg"));
    ui->acceptMarkButton->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->acceptMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->acceptMarkButton->hide();

    // Кнопка удаления метки
    ui->rejectMarkButton->setIcon(QIcon(":/icons/reject.svg"), QIcon(":/icons/reject_pressed.svg"));
    ui->rejectMarkButton->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->rejectMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->rejectMarkButton->hide();

    // Кнопки интервалов
    ui->intervalButton->setIcon(QIcon(":/icons/startInterval1.svg"), QIcon(":/icons/startInterval1_pressed.svg"));
    ui->intervalButton->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->intervalButton->setStyleSheet(ToolButtonStyleSheet);
    ui->intervalButton->hide();

    // Кнопка подтверждения интервала
    ui->acceptIntervalButton->setIcon(QIcon(":/icons/accept.svg"), QIcon(":/icons/accept_pressed.svg"));
    ui->acceptIntervalButton->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->acceptIntervalButton->setStyleSheet(ToolButtonStyleSheet);
    ui->acceptIntervalButton->hide();

    // Кнопка удаления интервала
    ui->rejectIntervalButton->setIcon(QIcon(":/icons/reject.svg"), QIcon(":/icons/reject_pressed.svg"));
    ui->rejectIntervalButton->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->rejectIntervalButton->setStyleSheet(ToolButtonStyleSheet);
    ui->rejectIntervalButton->hide();

    // Кнопка перехода к 1 интервалу
    ui->goToInterval1Button->setIcon(QIcon(":/icons/firstInterval.svg"),QIcon(":/icons/firstInterval_pressed.svg"));
    ui->goToInterval1Button->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->goToInterval1Button->setStyleSheet(ToolButtonStyleSheet);
    ui->goToInterval1Button->hide();

    // Кнопка перехода ко 2 интервалу
    ui->goToInterval2Button->setIcon(QIcon(":/icons/secondInterval.svg"),QIcon(":/icons/secondInterval_pressed.svg"));
    ui->goToInterval2Button->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->goToInterval2Button->setStyleSheet(ToolButtonStyleSheet);
    ui->goToInterval2Button->hide();

    ui->goToNextMarkButton->setIcon(QIcon(":/icons/nextLabel.svg"),QIcon(":/icons/nextLabel_pressed.svg"));
    ui->goToNextMarkButton->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->goToNextMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->goToNextMarkButton->show();


    ui->goToPreviousMarkButton->setIcon(QIcon(":/icons/previousLabel.svg"),QIcon(":/icons/previousLabel_pressed.svg"));
    ui->goToPreviousMarkButton->setIconSize(QSize(BUT_SIZE_BIG, BUT_SIZE_BIG));
    ui->goToPreviousMarkButton->setStyleSheet(ToolButtonStyleSheet);
    ui->goToPreviousMarkButton->show();

    ui->goToPreviousMarkButton->hide();
    ui->goToNextMarkButton->hide();
    ui->labelsNavigation->hide();

    ui->maxValueInterval1->hide();
    ui->maxValueInterval2->hide();

    ui->alarmLevelICPWidget->hide();
    ui->averageICPWidget->hide();
    ui->sensorStateLabel->hide();

    ui->playRecord->setIcon(QIcon(":/icons/playRecord.svg"),QIcon(":/icons/playRecord_pressed.svg"));
    ui->playRecord->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->playRecord->setStyleSheet(ToolButtonStyleSheet);
    ui->playRecord->hide();

    ui->rewindRecordButton->setIcon(QIcon(":/icons/rewindRecord.svg"),QIcon(":/icons/rewindRecord_pressed.svg"));
    ui->rewindRecordButton->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->rewindRecordButton->setStyleSheet(ToolButtonStyleSheet);
    ui->rewindRecordButton->hide();

    ui->speedRecordButton->setIcon(QIcon(":/icons/speedX1.svg"),QIcon(":/icons/speedX1_pressed.svg"));
    ui->speedRecordButton->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->speedRecordButton->setStyleSheet(ToolButtonStyleSheet);
    ui->speedRecordButton->hide();

    ui->downloadGraphButton->setIcon(QIcon(":/icons/downloadGraph.svg"),QIcon(":/icons/downloadGraph_pressed.svg"));
    ui->downloadGraphButton->setIconSize(QSize(BUT_SIZE_SMALL, BUT_SIZE_SMALL));
    ui->downloadGraphButton->setStyleSheet(ToolButtonStyleSheet);
    ui->downloadGraphButton->hide();
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
    if (!controller) { return; }

    mController = controller;

    //ui->averageICPWidget->installController(mController);
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
//    case ControllerEvent::SessionStarted: {
//        //updateSessionInfo(); !!!!
//        break;
//    }
//    case ControllerEvent::SessionClosed: {
//        //updateSessionInfo(); !!!!
//        break;
//    }
    case ControllerEvent::GlobalTimeUpdate: {
        updateDateTime();
        break;
    }
    case ControllerEvent::LabelCreated: {
        //updateLabelCounter();
        break;
    }
//    case ControllerEvent::SensorConnected: {
//        mLastSensorState = ControllerEvent::SensorConnected;
//        //updateSensorState();
//        break;
//    }
//    case ControllerEvent::SensorConnectionError: {
//        mLastSensorState = ControllerEvent::SensorConnectionError;
//        //updateSensorState();
//        break;
//    }
//    case ControllerEvent::SensorReset: {
//        mLastSensorState = ControllerEvent::SensorReset;
//        //updateSensorState();
//        break;
//    }
//    case ControllerEvent::SensorResetError: {
//        mLastSensorState = ControllerEvent::SensorResetError;
//        //updateSensorState();
//        break;
//    }
//    case ControllerEvent::SensorDisconnected: {
//        mLastSensorState = ControllerEvent::SensorDisconnected;
//        //updateSensorState();
//        break;
//    }
//    case ControllerEvent::SoftwareStorageAvailable: {
//        updateSoftwareStorageOnWidgets(true);
//        break;
//    }
//    case ControllerEvent::SoftwareStorageUnavailable: {
//        updateSoftwareStorageOnWidgets(false);
//        break;
//    }
//    case ControllerEvent::SoftwareStorageNotAssigned: {
//        updateSoftwareStorageOnWidgets(false);
//        break;
//    }
//    case ControllerEvent::DataBaseAvailable: {
////!!!   ui->makeLabelButton->setDisabled(false);
//        ui->softwareStorageIconSVG->load(QString(":/icons/softwareStorage.svg"));
//        break;
//    }
//    case ControllerEvent::DataBaseUnavailable: {
////!!!   ui->makeLabelButton->setDisabled(true);
//        ui->softwareStorageIconSVG->load(QString(":/icons/softwareStorageUnavailable.svg"));
//        break;
//    }
//    case ControllerEvent::ScreenWritten:
//    case ControllerEvent::ScreenNotWritten: {
//        ui->photoButton->setEnabled(true);
//        break;
//    }
    default: break;
    }
}

//void MainPage::updateSensorInfo()
//{
//    // Если контроллер не установлен
//    if (!mController) {
//        return;
//    }

//    // Получаем текущий датчик
//    const auto sensor = mController->sensor();
//    if (!sensor) { return; }

////    // Устанавливаем информацию о датчике
////    ui->sensorInfoLabel->setText(sensor->info());
//}

//void MainPage::updateSensorResetTime()
//{
//    // Если контроллер не установлен
//    if (!mController) { return; }

//    // Получаем текущий датчик
//    const ISensor * const sensor = mController->sensor();
//    if (!sensor) { return; }

//    // Обновляем информацию
//    ui->sensorStateLabel->setText(QString("%1: %2").arg(tr("Обнулён")).arg(QDateTime::fromMSecsSinceEpoch(sensor->lastResetTimestamp()).toString("hh:mm:ss-dd.MM.yyyy")));
//}

//void MainPage::updateSensorState()
//{
//    switch (mLastSensorState) {
////    case ControllerEvent::SensorConnected: {
////        ui->sensorStateLabel->setText(tr("Датчик подключён"));
////        updateSensorInfo();
////        break;
////    }
////    case ControllerEvent::SensorConnectionError: {
////        ui->sensorStateLabel->setText(tr("Ошибка соединения с датчиком"));
////        break;
////    }
////    case ControllerEvent::SensorReset: {
////        updateSensorResetTime();
////        break;
////    }
////    case ControllerEvent::SensorResetError: {
////        ui->sensorStateLabel->setText(tr("Ошибка сброса датчика"));
////        break;
////    }
////    case ControllerEvent::SensorDisconnected: {
////        ui->sensorStateLabel->setText(tr("Датчик отключён"));
////        updateSensorInfo();
////        break;
////    }
//        default: break;
//    }
//}

//void MainPage::updateSessionInfo()
//{
//    // Если контроллер не установлен
//    if (!mController) { return; }

//    // Получаем текущую сессию
//    //const auto &currentSession = mController->getCurrentSession();
////    if (!currentSession) {
////        ui->sessionID->setText(tr("Сессия неактивна!"));
////    } else {
////        ui->sessionID->setText(QString("%1 %2").arg(tr("ID сессии:")).arg(currentSession->getId()));
////    }
//}

void MainPage::updateLabelCounter()
{
    // Если контроллер не установлен
    if (!mController) { return; }

    // Подготовка шаблона для заполнения
    const QString counterLabelText = tr("Меток: %1");
    const QString currentLabelText = tr("%1/%2");
    // Запрос у контроллера кол-ва меток за текущую сессию
    int countLabels = mController->getLabelsCountPerCurrentSession();
    if (countLabels > 0)
    {
        //ui->labelCounterLabel->setText(counterLabelText.arg(mLabelManagerGlobal->mCountLabels));
        ui->labelsNavigation->setText(currentLabelText.arg(mCurrentLabelIndex+1).arg(mLabelManagerGlobal->mCountLabels));
    } else {
        //ui->labelCounterLabel->setText(tr("Меток: "));
    }
}

void MainPage::scaleFont(float scaleFactor)
{
    WFontScaling(ui->dateTimeLabel, scaleFactor);
    WFontScaling(ui->sensorStateLabel, scaleFactor);
    ui->alarmLevelICPWidget->scaleFont(scaleFactor);
    mCurrentGraphsArea->scaleFont(scaleFactor);
    mMainMenu->scaleFont(scaleFactor);
    WFontScaling(ui->labelsNavigation, scaleFactor);
    WFontScaling(ui->maxValueInterval1, scaleFactor);
    WFontScaling(ui->maxValueInterval2, scaleFactor);
    WFontScaling(ui->averageValue, scaleFactor);
    //WFontScaling(ui->sensorInfoLabel, scaleFactor);
    //WFontScaling(ui->labelCounterLabel, scaleFactor);
    //WFontScaling(ui->sessionID, scaleFactor);
    //ui->averageICPWidget->scaleFont(scaleFactor);
    //WFontScaling(ui->averageValueInterval1, scaleFactor);
    //WFontScaling(ui->averageValueInterval2, scaleFactor);

}

void MainPage::updateDateTime()
{
    auto currentDateTime = QDateTime::currentDateTime().toString("hh:mm\ndd.MM.yyyy");
    if (currentDateTime != mLastDateTime) {
        ui->dateTimeLabel->setText(currentDateTime);
        mLastDateTime = currentDateTime;

//        if (mController) {
//            QTimer::singleShot(0, mController, [this](){
//                mController->saveCurrentDateTime();
//            });
//        }
    }
}

//void MainPage::updateSoftwareStorageOnWidgets(bool isAvailable)
//{
//    QString svgPath;
//    if (isAvailable) {
//        svgPath = ":/icons/softwareStorage.svg";
//    } else {
//        svgPath = ":/icons/softwareStorageUnavailable.svg";
//    }

//    ui->softwareStorageIconSVG->load(svgPath);
//}

//void MainPage::makeScreen()
//{
//    if (!mController) {
//        return;
//    }

////#ifdef PC_BUILD
////    const QPixmap &&screen(makeAScreenOfWidget(this));
////    if (!screen.isNull()) {
////        ui->photoButton->setDisabled(true);
////        QTimer::singleShot(0, mController, [this, scr = std::move(screen)](){
////            mController->writeScreenFromWidget(scr);
////        });
////    }
////#else
////    QTimer::singleShot(0, mController, [this](){
////        ui->photoButton->setDisabled(true);
////        mController->writeScreenFromLinuxFB();
////    });
////#endif
//}

void MainPage::nextGraph()
{
    // Кол-во графиков
    int countGraph = mGraphWidgetContainer.size();

    // Если в списке меньше одного графика
    if (countGraph < 2) { return; }

    // Скрываем текущий график
    mGraphWidgetContainer[mGraphNumber]->hide();

    // Показываем следующий график
    ++mGraphNumber;
    mGraphNumber %= countGraph;
    mGraphWidgetContainer[mGraphNumber]->show();
}

void MainPage::showEvent(QShowEvent */*event*/)
{
    updateDateTime();
    mUpdateDateTimeTimer->start();
}

void MainPage::hideEvent(QHideEvent */*event*/)
{
    mUpdateDateTimeTimer->stop();
}

void MainPage::retranslate()
{
    //ui->averageICPWidget->retranslate();
    mCurrentGraphsArea->retranslate();
    //updateSessionInfo(); !!!!
    //updateSensorState();
    //updateSensorInfo();
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
        //mSensorDataManager->isStopSensorData = true;
        mCurrentGraphsArea->stopWork();
        emit (resetWaveGraph());
        mCurrentGraphsArea->startWork();
        isStart = false;
        mCurrentRecordDir.setPath(mntDirectory+ "/" + currentTime);
        ui->recordButton->setIcon(QIcon(":/icons/stopRecord.svg"), QIcon(":/icons/stopRecord_pressed.svg"));

        mHeadFile.setFileName(mCurrentRecordDir.path()      + "/" + "HEAD.txt");
        mIntervalsFile.setFileName(mCurrentRecordDir.path() + "/" + "INTERVALS.txt");
        mMarksFile.setFileName(mCurrentRecordDir.path()     + "/" + "MARKS.txt");
        mRawDataFile.setFileName(mCurrentRecordDir.path()   + "/" + "RAW_DATA.txt");

        qDebug() << mCurrentRecordDir.path();
        if (!mCurrentRecordDir.exists())
        {
#ifdef TEST_BUILD
            QString response;
            response = executeAConsoleCommand("mkdir", QStringList() << mCurrentRecordDir.path());
            if (response == "")
            {
                qDebug() << "Dir open = "<< mCurrentRecordDir.path();
            }
            else
            {
                qDebug() << "error" << response;
            }
#endif
#ifdef PC_BUILD
            mCurrentRecordDir.mkdir(mCurrentRecordDir.path());
#endif
        }
        if (mHeadFile.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            QStringList time = currentTime.split("@");
            mHeadFile.write(("Session started: " + time[0].replace("_", ".") + " " + time[1].replace("_", ":") +
                    "\nStartTimeStamp(ms): " + QString::number(getCurrentTimeStampMS()) + "\n").toLatin1());
            mHeadFile.close();
        }
        ui->mainWidgets   ->show();
        ui->sessionButton ->setEnabled(false);

        //mSensorDataManager->startRecord();
        mCurrentGraphsArea->isRecord = true;
        emit (resetWaveGraph());


    }
    else
    {
        uint64_t StopMS = getCurrentTimeStampMS();
        ui->recordButton->setIcon(QIcon(":/icons/startRecord.svg"), QIcon(":/icons/startRecord_pressed.svg"));
        currentTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
        if (mHeadFile.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            QStringList time = currentTime.split("@");
            mHeadFile.write(("Session stopped: " + currentTime + "\nStopTimeStamp(ms): " + QString::number(StopMS) + "\n").toLatin1());
            mHeadFile.close();
        }
        isStart = true;

        mCurrentGraphsArea->isRecord = false;
        emit(dataReadyForRecordGraph());


#define SET_VISIBLED_DISABLED(_UUII) {ui->_UUII->show();  ui->_UUII->setEnabled(false);}
#define SET_VISIBLED_ENABLED(_UUII) {ui->_UUII->show();  ui->_UUII->setEnabled(true);}

        ui->intervalButton      ->show();
        ui->playRecord          ->show();
        ui->rewindRecordButton  ->show();
        ui->speedRecordButton   ->show();
        ui->downloadGraphButton ->show();

        ui->goToInterval1Button ->hide();
        ui->maxValueInterval1   ->hide();
        ui->goToInterval2Button ->hide();
        ui->maxValueInterval2   ->hide();
        ui->recordButton        ->hide();
        ui->alarmLevelICPWidget ->hide();
        ui->averageValue        ->hide();

        ui->sessionButton       ->setEnabled(true);


        SET_VISIBLED_DISABLED(goToPreviousMarkButton);
        SET_VISIBLED_DISABLED(labelsNavigation);
        ui->labelsNavigation->setText("0/0");
        SET_VISIBLED_DISABLED(goToNextMarkButton);
        SET_VISIBLED_ENABLED(makeLabelButton);

        //ui->averageICPWidget->hide();

        // Остановка считывания данных
        //mSensorDataManager->isRunning = false;
        //while(mSensorDataManager->isStopped == false) {}

        mCurrentGraphsArea->stopWork();
        emit (changeCurrentGraph());
    }
}


void MainPage::on_makeLabelButton_clicked()
{
    ui->acceptMarkButton        ->show();
    ui->rejectMarkButton        ->show();
    ui->makeLabelButton         ->setEnabled(false);
    ui->sessionButton           ->setEnabled(false);
    ui->goToPreviousMarkButton  ->setEnabled(false);
    ui->goToNextMarkButton      ->setEnabled(false);
    ui->goToInterval1Button     ->setEnabled(false);
    ui->goToInterval2Button     ->setEnabled(false);
    ui->intervalButton          ->setEnabled(false);
    ui->playRecord              ->setEnabled(false);
    ui->rewindRecordButton      ->setEnabled(false);
    ui->speedRecordButton       ->setEnabled(false);
    ui->downloadGraphButton     ->setEnabled(false);

    QTimer::singleShot(0, mController, [this](){ mController->makeLabel(); });

    //emit (changeLabelButtonStatus);
}

void MainPage::on_acceptMarkButton_clicked()
{
    ui->labelsNavigation    ->show();
    ui->acceptMarkButton    ->hide();
    ui->rejectMarkButton    ->hide();
    ui->makeLabelButton     ->setEnabled(true);
    ui->intervalButton      ->setEnabled(true);
    ui->sessionButton       ->setEnabled(true);
    ui->goToInterval1Button ->setEnabled(true);
    ui->goToInterval2Button ->setEnabled(true);
    ui->playRecord          ->setEnabled(true);
    ui->rewindRecordButton  ->setEnabled(true);
    ui->speedRecordButton   ->setEnabled(true);
    ui->downloadGraphButton ->setEnabled(true);

    mMarksFile.open(QIODevice::WriteOnly | QIODevice::Append);
    mMarksFile.write((QString::number(mLabelManagerGlobal->mCountLabels) + ": " + QString::number(mCoordLabelX) + "\n").toLatin1());
    mMarksFile.close();
    mLabelItemsContainer.back()->getLabel()->mCurrentPos = (double)mCoordLabelX/1000;

    if (mIntervalsCount < 4) { ui->intervalButton->show(); }
    emit(changeRecordedGraphInteraction(true));
    updateLabelCounter();
    if (ui->labelsNavigation->text() != "0/0")
    {
        ui->goToPreviousMarkButton  ->setEnabled(true);
        ui->goToNextMarkButton      ->setEnabled(true);
    }
}


void MainPage::on_rejectMarkButton_clicked()
{
    ui->acceptMarkButton    ->hide();
    ui->rejectMarkButton    ->hide();
    ui->makeLabelButton     ->setEnabled(true);
    ui->intervalButton      ->setEnabled(true);
    ui->sessionButton       ->setEnabled(true);
    ui->goToInterval1Button ->setEnabled(true);
    ui->goToInterval2Button ->setEnabled(true);
    ui->playRecord          ->setEnabled(true);
    ui->rewindRecordButton  ->setEnabled(true);
    ui->speedRecordButton   ->setEnabled(true);
    ui->downloadGraphButton ->setEnabled(true);

    if (mIntervalsCount < 4) { ui->intervalButton->show(); }
    emit(changeRecordedGraphInteraction(false));
    if (ui->labelsNavigation->text() != "0/0")
    {
        ui->goToPreviousMarkButton  ->setEnabled(true);
        ui->goToNextMarkButton      ->setEnabled(true);
    }
}

void MainPage::startSession()
{
  ui->sessionButton->setIcon(QIcon(":/icons/deleteSession.svg"), QIcon(":/icons/deleteSession_pressed.svg"));
  //mSensorDataManager = new SensorDataManager(this, mCurrentRecordDir.path());
  //connect(mSensorDataManager, &SensorDataManager::printDataOnGraph, mCurrentGraphsArea, &CurrentGraphsArea::addDataOnWavePlot);
  connect(mCurrentGraphsArea, &CurrentGraphsArea::averageReady, this, &MainPage::setAverage);
  connect(this, &MainPage::setAveragePointerPos, ui->alarmLevelICPWidget, &AlarmLevelICPWidget::updateAverageValueOnWidgets);

  //mSensorDataManager->start();

  ui->recordButton        ->show();
  ui->alarmLevelICPWidget ->show();
  ui->averageValue        ->show();
  ui->homeButton          ->hide();
  ui->makeLabelButton     ->hide();
  ui->acceptMarkButton    ->hide();
  ui->rejectMarkButton    ->hide();
  ui->recordButton        ->setEnabled(true);

  //ui->averageICPWidget->show();
  mCurrentLabelIndex = 0;
  mCurrentGraphsArea->startPlotting();
  mCurrentGraphsArea->startWork();
}
void MainPage::stopSession()
{
  ui->sessionButton->setIcon(QIcon(":/icons/newSession.svg"), QIcon(":/icons/newSession_pressed.svg"));
  //mCurrentGraphsArea->stopPlotting();
  //mController->closeSession();

  emit (changeCurrentGraph());
  emit (resetWaveGraph());

  ui->homeButton            ->show();
  ui->makeLabelButton       ->hide();
  ui->acceptMarkButton      ->hide();
  ui->rejectMarkButton      ->hide();
  ui->recordButton          ->hide();
  ui->intervalButton        ->hide();
  ui->acceptIntervalButton  ->hide();
  ui->rejectIntervalButton  ->hide();
  ui->goToInterval1Button   ->hide();
  ui->goToInterval2Button   ->hide();
  ui->labelsNavigation      ->hide();
  ui->goToPreviousMarkButton->hide();
  ui->goToNextMarkButton    ->hide();
  ui->maxValueInterval1     ->hide();
  ui->maxValueInterval2     ->hide();
  ui->averageValue          ->hide();
  ui->alarmLevelICPWidget   ->hide(); //или оставить?
  ui->playRecord            ->hide();
  ui->rewindRecordButton    ->hide();
  ui->speedRecordButton     ->hide();
  ui->downloadGraphButton   ->hide();
  //ui->averageValueInterval1->hide();
  //ui->averageValueInterval2->hide();
  //ui->averageICPWidget->hide();
  //        ui->averageValue->setText("--");
  //        ui->averageValue->show();// или скрывать вообще?

  // Остановка потока SensorDataManager - считывание данных
  //mSensorDataManager->isRunning = false;
  //while(mSensorDataManager->isStopped == false) {}

  mCurrentGraphsArea->stopWork();
}
void MainPage::on_sessionButton_clicked()
{
  if (isSessionStart)
  {
    isSessionStart = false;
    startSession();
  }
  else
  {
    isSessionStart = true;
    stopSession();
  }
}
void MainPage::on_intervalButton_clicked()
{
    ui->acceptIntervalButton    ->show();
    ui->rejectIntervalButton    ->show();
    ui->makeLabelButton         ->setEnabled(false); //ui->makeLabelButton->hide();
    ui->sessionButton           ->setEnabled(false);
    ui->goToPreviousMarkButton  ->setEnabled(false);
    ui->goToNextMarkButton      ->setEnabled(false);
    ui->goToInterval1Button     ->setEnabled(false);
    ui->goToInterval2Button     ->setEnabled(false);
    ui->playRecord              ->setEnabled(false);
    ui->rewindRecordButton      ->setEnabled(false);
    ui->speedRecordButton       ->setEnabled(false);
    ui->downloadGraphButton     ->setEnabled(false);

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

    ui->acceptIntervalButton  ->hide();
    ui->rejectIntervalButton  ->hide();
    ui->makeLabelButton       ->setEnabled(true); //ui->makeLabelButton->show();
    ui->sessionButton         ->setEnabled(true);
    ui->goToInterval1Button   ->setEnabled(true);
    ui->goToInterval2Button   ->setEnabled(true);
    ui->intervalButton        ->setEnabled(true);
    ui->playRecord            ->setEnabled(true);
    ui->rewindRecordButton    ->setEnabled(true);
    ui->speedRecordButton     ->setEnabled(true);
    ui->downloadGraphButton   ->setEnabled(true);

    emit(changeRecordedGraphInteraction(true));

    //const QString maxValuePreset = tr("Максимум\n%1"); //перевести потом
    //const QString averageValuePreset = tr("Среднее\n%1"); //перевести потом

    const QString intervalDataPreset = tr("Максимум\n%1\nСреднее\n%2"); //перевести потом
    //QString maxValuePreset = tr("Максимум"); //перевести потом
    //QString averageValuePreset = tr("Среднее"); //перевести потом

    if (mIntervalsCount % 2 == 0)
    {
        mCurrentGraphsArea->colorInterval();
        if (mIntervalsCount == 2)
        {
            ui->maxValueInterval1->setText(intervalDataPreset.arg(mIntervalsContainer[mIntervalsCount-1]->maxIntervalValue, 0, 'f', 1).arg(mIntervalsContainer[mIntervalsCount-1]->averageIntervalValue, 0, 'f', 1));
            ui->goToInterval1Button ->show();
            ui->maxValueInterval1   ->show();
        }
        else
        {
            ui->maxValueInterval2->setText(intervalDataPreset.arg(QString::number(mIntervalsContainer[mIntervalsCount-1]->maxIntervalValue, 'f', 1)).arg(QString::number(mIntervalsContainer[mIntervalsCount-1]->averageIntervalValue, 'f', 1)));
            ui->goToInterval2Button ->show();
            ui->maxValueInterval2   ->show();
            ui->intervalButton      ->hide();
        }
    }
    if (ui->labelsNavigation->text() != "0/0")
    {
        ui->goToPreviousMarkButton  ->setEnabled(true);
        ui->goToNextMarkButton      ->setEnabled(true);
    }
}


void MainPage::on_rejectIntervalButton_clicked()
{
    ui->acceptIntervalButton  ->hide();
    ui->rejectIntervalButton  ->hide();
    ui->makeLabelButton       ->setEnabled(true);
    ui->sessionButton         ->setEnabled(true);
    ui->goToInterval1Button   ->setEnabled(true);
    ui->goToInterval2Button   ->setEnabled(true);
    ui->intervalButton        ->setEnabled(true);
    ui->playRecord            ->setEnabled(true);
    ui->rewindRecordButton    ->setEnabled(true);
    ui->speedRecordButton     ->setEnabled(true);
    ui->downloadGraphButton   ->setEnabled(true);

    if (ui->labelsNavigation->text() != "0/0")
    {
        ui->goToPreviousMarkButton  ->setEnabled(true);
        ui->goToNextMarkButton      ->setEnabled(true);
    }
    mIntervalsCount--;
    emit(changeRecordedGraphInteraction(false));

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


void MainPage::on_playRecord_clicked()
{
    if (isPlayRecord)
    {
        isPlayRecord = false;
        ui->playRecord->setIcon(QIcon(":/icons/pauseRecord.svg"),QIcon(":/icons/pauseRecord_pressed.svg"));
        ui->intervalButton->setEnabled(false);
        ui->goToInterval1Button->setEnabled(false);
        ui->goToInterval2Button->setEnabled(false);
        ui->goToNextMarkButton->setEnabled(false);
        ui->goToPreviousMarkButton->setEnabled(false);
        ui->makeLabelButton->setEnabled(false);
        ui->sessionButton->setEnabled(false);
        ui->rewindRecordButton->setEnabled(false);
        ui->speedRecordButton->setEnabled(false);
        ui->downloadGraphButton->setEnabled(false);
    }
    else
    {
        isPlayRecord = true;
        ui->playRecord->setIcon(QIcon(":/icons/playRecord.svg"),QIcon(":/icons/playRecord_pressed.svg"));
        ui->intervalButton->setEnabled(true);
        ui->goToInterval1Button->setEnabled(true);
        ui->goToInterval2Button->setEnabled(true);
        ui->goToNextMarkButton->setEnabled(true);
        ui->goToPreviousMarkButton->setEnabled(true);
        ui->makeLabelButton->setEnabled(true);
        ui->sessionButton->setEnabled(true);
        ui->rewindRecordButton->setEnabled(true);
        ui->speedRecordButton->setEnabled(true);
        ui->downloadGraphButton->setEnabled(true);
    }

    emit(playBtnPressed());
}


void MainPage::on_rewindRecordButton_clicked()
{
    mCurrentGraphsArea->mRecordedGraph->xAxis->setRange(0, mCurrentGraphsArea->mRecordedGraph->xAxis->range().size());
}


void MainPage::on_downloadGraphButton_clicked()
{
    mCurrentGraphsArea->mRecordedGraph->clearItems();
    mCurrentGraphsArea->mRecordedGraph->clearGraphs();
    QFile mTestData("/media/test/testData.txt");
    mTestData.open(QIODevice::ReadOnly);
    QByteArray temp = mTestData.readAll();
    mTestData.close();
    mCurrentGraphsArea->mRecordedGraph->downloadData(&temp);//доделать
}


void MainPage::on_speedRecordButton_clicked()
{
    iconCount++;
    switch (iconCount % 3)
    {
        case 0: //normal
        {
            currSpeed = speed::SpeedX1;
            ui->speedRecordButton->setIcon(QIcon(":/icons/speedX1.svg"),QIcon(":/icons/speedX1_pressed.svg"));
            break;
        }
        case 1: //x2
        {
            currSpeed = speed::SpeedX2;
            ui->speedRecordButton->setIcon(QIcon(":/icons/speedX2.svg"),QIcon(":/icons/speedX2_pressed.svg"));
            break;
        }
        case 2: //x4
        {
            currSpeed = speed::SpeedX2;
            ui->speedRecordButton->setIcon(QIcon(":/icons/speedX4.svg"),QIcon(":/icons/speedX4_pressed.svg"));
            break;
        }
        default : break;
    }
}

void MainPage::setAverage(double currAverage)
{
    ui->averageValue->setText(QString::number(round(currAverage)));
    setAveragePointerPos(currAverage);
}
