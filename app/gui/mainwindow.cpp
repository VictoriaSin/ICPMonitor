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

MainWindow::MainWindow( QWidget *parent) : QWidget(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    mMainPage = new MainPage(this);    
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
    qDebug("\033[34m>>MainWindow::MainWindow\033[0m");
}
MainWindow::~MainWindow()
{
    delete ui;
    DESTROY_CLASS(mMainPage);
    qDebug("\033[34m<<MainWindow::~MainWindow\033[0m");

}
void MainWindow::destroyMonitorController()
{
  QTimer::singleShot(100, [this]()
  {
    DESTROY_CLASS(mController);
    mControllerThread.quit();
    mControllerThread.wait(100000);
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
    scaleFonts();
    setZeroSensorPage();
}

void MainWindow::scaleFonts()
{
    if (!mController) { return; }
    const Settings * const settings = mController->settings();
    if (!settings) { return; }
    const float scaleFactor = settings->getFontScaleFactor();
    mMainPage->scaleFont(scaleFactor);
}
bool MainWindow::changeCurrentPage(IPageWidget *installedPage)
{
    if (mCurrentPage == installedPage) { return false; }
    if (mCurrentPage)
    {
        mCurrentPage->hide();
        ui->page->removeWidget(mCurrentPage);
    }
    mCurrentPage = installedPage;
    if (mCurrentPage)
    {
        ui->page->addWidget(mCurrentPage);
        mCurrentPage->show();
    }
    return mCurrentPage != nullptr;
}
void MainWindow::setZeroSensorPage()
{
    if (!mController) { return; }

    const Settings * const settings = mController->settings();
    if (!settings) { return; }

    ZeroSensorPage *setZeroSensorPage = new ZeroSensorPage();
    setZeroSensorPage->installController(mController);
    setZeroSensorPage->enableRejectButton(false); // Прячем кнопку возвращения назад
    setZeroSensorPage->scaleFont(settings->getFontScaleFactor());

    connect(setZeroSensorPage, &DateTimePage::previousPage, this, [this, setZeroSensorPage]()
    {
        setPreviousPage();
        setZeroSensorPage->deleteLater();
    });
    setPage(setZeroSensorPage);
}
void MainWindow::controllerEventHandler(ControllerEvent event, const QVariantMap &args)
{
    // Если есть какое-то сообщение от контроллера
    if (args.contains(GlobalEventsArgs::Message))
    {
        const QString message = args[GlobalEventsArgs::Message].toString();
        if (message.isEmpty()) { return; }
    }
    if (event == ControllerEvent::UpdateGeneralGroupInfo) { scaleFonts(); }
}
void MainWindow::setPage(IPageWidget *installedPage)
{
    // Заменяем текущую страницу на устанавливаемую, если возможно
    if (changeCurrentPage(installedPage))
    {
        mStackOfWidgets.push(installedPage); // Добавляем в стек страниц
    }
}
void MainWindow::setPreviousPage()
{
    // Если в стэке больше одной страницы
    if (mStackOfWidgets.size() > 1)
    {
        mStackOfWidgets.pop(); // Удаляем текущую страницу из стека
        changeCurrentPage(mStackOfWidgets.top()); // Заменяем отображение текущей страницы на предыдущую
    }
}
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        mMainPage->retranslate();
    }
    else
    {
        QWidget::changeEvent(event);
    }
}
