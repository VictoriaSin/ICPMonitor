#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller/settings.h"
#include "gui/mainpage.h"
#include "gui/datetimepage.h"
#include "controller/monitorcontroller.h"
#include "dialogWindows/messagedialog.h"
#include "gui/zerosensorpage.h"
//#include "mainmenu.h"

#include <QDebug>

MainWindow::MainWindow( QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , mMainPage(new MainPage(this))
    , mMessageDialog(new MessageDialog(this))
{
    ui->setupUi(this);

    // Скрываем виджет для сообщений
    mMessageDialog->hide();

    // Принимаем события тачскрина
    setAttribute(Qt::WA_AcceptTouchEvents);

    // Устанавливаем текущую страницу
    setPage(mMainPage);

    // Замена виджета
    connect(mMainPage, &IPageWidget::changePage, this, &MainWindow::setPage);
    connect(mMainPage, &IPageWidget::previousPage, this, &MainWindow::setPreviousPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::installController(MonitorController *controller)
{
    if (!controller) {
        return;
    }    
    mController = controller;
    mMainPage->installController(mController);

    // Обработка событий контроллера
    connect(mController, &MonitorController::controllerEvent, this, &MainWindow::controllerEventHandler);

    // Переводим приложение
    retranslate();

    // Скейлим шрифты
    scaleFonts();

    setZeroSensorPage();
    // Если текущая дата не валидная, то выводим экран установки даты
    if (!mController->currentTimeIsValid())
    {
        setCurrentDatePage();
    }
}

void MainWindow::retranslate()
{
    mMainPage->retranslate();
    mMessageDialog->retranslate();
}

void MainWindow::scaleFonts()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings)
    {
        return;
    }

    const float scaleFactor = settings->getFontScaleFactor();
    mMainPage->scaleFont(scaleFactor);
    mMessageDialog->scaleFont(scaleFactor);
}

bool MainWindow::changeCurrentPage(IPageWidget *installedPage)
{
    if (mCurrentPage == installedPage) {
        return false;
    }

    if (mCurrentPage) {
        mCurrentPage->hide();
        ui->page->removeWidget(mCurrentPage);
    }

    mCurrentPage = installedPage;

    if (mCurrentPage) {
        ui->page->addWidget(mCurrentPage);
        mCurrentPage->show();
    }

    return mCurrentPage != nullptr;
}


void MainWindow::setZeroSensorPage()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    ZeroSensorPage *setZeroSensorPage = new ZeroSensorPage();
    setZeroSensorPage->installController(mController);
    //setZeroSensorPage->setBottomInfoLabel(tr("Для корректной работы устройства\nнеобходимо задать текущее время и дату"));
    setZeroSensorPage->enableRejectButton(false); // Прячем кнопку возвращения назад
    setZeroSensorPage->scaleFont(settings->getFontScaleFactor());

    connect(setZeroSensorPage, &DateTimePage::previousPage, this, [this, setZeroSensorPage](){
        setPreviousPage();
        setZeroSensorPage->deleteLater();
    });
    setPage(setZeroSensorPage);
}

void MainWindow::setCurrentDatePage()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    DateTimePage *settingsCurrentTimePage = new DateTimePage();
    settingsCurrentTimePage->installController(mController);
    settingsCurrentTimePage->setBottomInfoLabel(tr("Для корректной работы устройства\nнеобходимо задать текущее время и дату"));
    settingsCurrentTimePage->enableRejectButton(false); // Прячем кнопку возвращения назад
    settingsCurrentTimePage->scaleFont(settings->getFontScaleFactor());
    connect(settingsCurrentTimePage, &DateTimePage::previousPage, this, [this, settingsCurrentTimePage](){
        setPreviousPage();
        settingsCurrentTimePage->deleteLater();
    });
    setPage(settingsCurrentTimePage);
}

void MainWindow::controllerEventHandler(ControllerEvent event, const QVariantMap &args)
{
    // Если есть какое-то сообщение от контроллера
    if (args.contains(GlobalEventsArgs::Message)) {
        const QString message = args[GlobalEventsArgs::Message].toString();
        if (message.isEmpty()) {
            return;
        }
        mMessageDialog->setTextMessage(message);
        mMessageDialog->open();
    }
    if (event == ControllerEvent::UpdateGeneralGroupInfo)
    {
        scaleFonts();
    }
}

void MainWindow::setPage(IPageWidget *installedPage)
{
    // Заменяем текущую страницу на устанавливаемую, если возможно
    if (changeCurrentPage(installedPage)) {
        mStackOfWidgets.push(installedPage); // Добавляем в стек страниц
    }
}

void MainWindow::setPreviousPage()
{
    // Если в стэке больше одной страницы
    if (mStackOfWidgets.size() > 1) {
        mStackOfWidgets.pop(); // Удаляем текущую страницу из стека
        changeCurrentPage(mStackOfWidgets.top()); // Заменяем отображение текущей страницы на предыдущую
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        QWidget::changeEvent(event);
    }
}
