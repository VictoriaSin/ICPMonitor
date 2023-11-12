#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "controller/settings.h"
#include "gui/mainpage.h"
#include "gui/datetimepage.h"

#include "dialogWindows/messagedialog.h"
#include "gui/zerosensorpage.h"

#include <QDebug>
#include <QTimer>

MainPage *mMainPage = nullptr;

MainWindow::MainWindow( QWidget *parent) : QWidget(parent) , ui(new Ui::MainWindow) , mMessageDialog(new MessageDialog(this))
{
    ui->setupUi(this);
    mMainPage = new MainPage(this);
    mMessageDialog->hide();
    setAttribute(Qt::WA_AcceptTouchEvents); // Принимаем события тачскрина
    setPage(mMainPage);
    connect(mMainPage, &IPageWidget::changePage, this, &MainWindow::setPage);
    connect(mMainPage, &IPageWidget::previousPage, this, &MainWindow::setPreviousPage);
    connect(mMainPage, &MainPage::sessionStopped, this, &MainWindow::setZeroSensorPage);
    mController = new MonitorController;
    installController(mController);// Установка контроллера виджетам
    connect(mController,SIGNAL(destroyThread()),this, SLOT(destroyMonitorController()));
    mController->moveToThread(&mControllerThread);
    mControllerThread.start();

#ifdef RELEASE_BUILD
    mController->controllerEvent(ControllerEvent::GlobalTimeUpdate);
#endif
}
MainWindow::~MainWindow()
{
    delete ui;
    delete mMessageDialog;
    DESTROY_CLASS(mMainPage);
    qDebug() << "MainWindow::~MainWindow()";

}
void MainWindow::destroyMonitorController()
{
  QTimer::singleShot(100, [this]()
  {
    DESTROY_CLASS(mController);
    mControllerThread.quit();
    mControllerThread.wait(100000);
    qDebug("Close MainWindow");
    qApp->exit(0);
  });
}
void MainWindow::installController(MonitorController *controller)
{
    if (!controller) { return; }
    mController = controller;
    mMainPage->installController(mController);
    // Обработка событий контроллера
    connect(mController, &MonitorController::controllerEvent, this, &MainWindow::controllerEventHandler);
    mMainPage->retranslate();
    mMessageDialog->retranslate();
    scaleFonts();
    setZeroSensorPage();
}
void MainWindow::retranslate()
{
    //mMainPage->retranslate();
    //mMessageDialog->retranslate();
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
    if (args.contains(GlobalEventsArgs::Message))
    {
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
        //qDebug() << installedPage << installedPage->isEnabled() << installedPage->isHidden();
        mStackOfWidgets.push(installedPage); // Добавляем в стек страниц
        //qDebug() << "mStackOfWidgets" << mStackOfWidgets << mStackOfWidgets.size();
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
    if (event->type() == QEvent::LanguageChange)
    {
        mMainPage->retranslate();
        mMessageDialog->retranslate();
        //retranslate();
    }
    else
    {
        QWidget::changeEvent(event);
    }
}
