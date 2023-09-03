#include "mainmenu.h"
#include "gui/systeminfopage.h"
#include "gui/zerosensorpage.h"
#include "gui/downloadData/datadownloadpage.h"
#include "gui/datetimepage.h"
#include "gui/languagepage.h"

#include "gui/generalsettingspage.h"

#include "gui_funcs.h"
#include "controller/monitorcontroller.h"


#include <QAbstractButton>
#include <QDebug>
#include <QPushButton>
#include <QTimer>



MainMenu::MainMenu(QWidget *parent) :
    AbstractMenuPage(parent)
  , mSystemInfoPage(new SystemInfoPage(this))
  , mZeroSensorPage(new ZeroSensorPage(this))
  , mDataExportPage(new DataDownloadPage(this))
  , mDateTimePage(new DateTimePage(this))
  , mLanguagePage(new LanguagePage(this))
  , mGeneralSettingsPage(new GeneralSettingsPage(this))
{
    // Скрываем созданные окна
    mSystemInfoPage->hide();
    mZeroSensorPage->hide();
    mDataExportPage->hide();
    mDateTimePage->hide();
    mLanguagePage->hide();
    mGeneralSettingsPage->hide();

    // Общий стиль для кнопок
    const QString ToolButtonStyleSheet = readStyleSheetFile(":/styles/ToolButtons.qss");

    // Настраиваем кнопку назад
    setupBackButton(QIcon(":/icons/home.svg"), QIcon(":/icons/home_pressed.svg"), QSize(125, 125), ToolButtonStyleSheet);

    // Кнопки для верхнего GridLayout
    addSettingButton("Обнуление датчика", QIcon(":/icons/zeroReset.svg"), QIcon(":/icons/zeroReset_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mZeroSensorPage,        0, 0, PageID::ZeroSensor);
    addSettingButton("Язык", QIcon(":/icons/changeLanguage.svg"),                         QIcon(":/icons/changeLanguage_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mLanguagePage,          0, 1, PageID::Language);
    addSettingButton("Время/Дата", QIcon(":/icons/dateTime.svg"),                         QIcon(":/icons/dateTime_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mDateTimePage,          1, 0, PageID::DateTime);
    addSettingButton("Экспорт данных", QIcon(":/icons/downloadData.svg"),                 QIcon(":/icons/downloadData_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mDataExportPage,        1, 1, PageID::DataExport);
    addSettingButton("Системная информация", QIcon(":/icons/info.svg"),                   QIcon(":/icons/info_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mSystemInfoPage,        2, 0, PageID::SystemInfo);
    addSettingButton("Технический доступ", QIcon(":/icons/tools.svg"),             QIcon(":/icons/tools_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mGeneralSettingsPage,   2, 1, PageID::General);
    addSettingButton("Выключение", QIcon(":/icons/powerOff.svg"),                  QIcon(":/icons/powerOff_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, [this]() {Test();},     3, 0, PageID::PowerOff);
    // Смена виджета на предыдущий
    connect(mSystemInfoPage, &AbstractDialogPage::previousPage, this, &MainMenu::previousPage);
    connect(mZeroSensorPage, &ZeroSensorPage::previousPage, this, &MainMenu::previousPage);
    connect(mDataExportPage, &DataDownloadPage::previousPage, this, &MainMenu::previousPage);
    connect(mDataExportPage, &DataDownloadPage::changePage, this, &MainMenu::changePage);
    connect(mDateTimePage, &DateTimePage::previousPage, this, &MainMenu::previousPage);
    connect(mDateTimePage, &DateTimePage::changePage, this, &MainMenu::changePage);
    connect(mLanguagePage, &LanguagePage::previousPage, this, &MainMenu::previousPage);
    connect(mLanguagePage, &LanguagePage::changePage, this, &MainMenu::changePage);
    connect(mGeneralSettingsPage, &GeneralSettingsPage::previousPage, this, &MainMenu::previousPage);
    connect(mGeneralSettingsPage, &GeneralSettingsPage::changePage, this, &MainMenu::changePage);
}

MainMenu::~MainMenu()
{

}


void MainMenu::controllerEventHandler(ControllerEvent event)//, const QVariantMap &args)
{
    switch(event) {
    case ControllerEvent::SoftwareStorageAvailable: {
        mButtonGroup.button(PageID::DataExport)->setEnabled(true);
        break;
    }
    case ControllerEvent::SoftwareStorageUnavailable: {
        mButtonGroup.button(PageID::DataExport)->setDisabled(true);
        break;
    }
    case ControllerEvent::SoftwareStorageNotAssigned: {
        mButtonGroup.button(PageID::DataExport)->setDisabled(true);
        break;
    }
        default: break;
    }
}

void MainMenu::scaleFont(float scaleFactor)
{
    AbstractMenuPage::scaleFont(scaleFactor);
    mSystemInfoPage->scaleFont(scaleFactor);
    mZeroSensorPage->scaleFont(scaleFactor);
    mDataExportPage->scaleFont(scaleFactor);
    mDateTimePage->scaleFont(scaleFactor);
    mLanguagePage->scaleFont(scaleFactor);
    mGeneralSettingsPage->scaleFont(scaleFactor);
}

void MainMenu::installController(MonitorController *controller)
{
    AbstractMenuPage::installController(controller);
    mSystemInfoPage->installController(controller);
    mZeroSensorPage->installController(controller);
    mDataExportPage->installController(controller);
    mDateTimePage->installController(controller);
    mLanguagePage->installController(controller);
    mGeneralSettingsPage->installController(controller);

    connect(mController, &MonitorController::controllerEvent, this , &MainMenu::controllerEventHandler);

    retranslate();
}

void MainMenu::retranslate()
{
    AbstractMenuPage::retranslate();

    mButtonGroup.button(PageID::ZeroSensor)->setText(tr("Обнуление датчика"));
    mButtonGroup.button(PageID::DataExport)->setText(tr("Экспорт данных"));
    mButtonGroup.button(PageID::SystemInfo)->setText(tr("Системная информация"));
    mButtonGroup.button(DateTime)  ->setText(tr("Время/Дата"));
    mButtonGroup.button(Language)  ->setText(tr("Язык"));
    mButtonGroup.button(PageID::PowerOff)  ->setText(tr("Выключение"));
    mButtonGroup.button(PageID::General)  ->setText(tr("Технический доступ"));

    mZeroSensorPage->retranslate();
    mDataExportPage->retranslate();
    mSystemInfoPage->retranslate();
    mDateTimePage->retranslate();
    mLanguagePage->retranslate();
    mGeneralSettingsPage->retranslate();
}
