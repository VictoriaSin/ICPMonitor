#include "mainmenu.h"
#include "gui/systeminfopage.h"
#include "gui/zerosensorpage.h"
#include "gui/datetimepage.h"
#include "gui/languagepage.h"
#include "gui/exportdatapage.h"
#include "gui/generalsettingspage.h"

#include "gui_funcs.h"
#include "controller/monitorcontroller.h"

#include <QAbstractButton>
#include <QDebug>
#include <QPushButton>

MainMenu::MainMenu(QWidget *parent) :
    AbstractMenuPage(parent)
  , mSystemInfoPage(new SystemInfoPage(this))
  , mZeroSensorPage(new ZeroSensorPage(this))
  , mDateTimePage(new DateTimePage(this))
  , mLanguagePage(new LanguagePage(this))
  , mGeneralSettingsPage(new GeneralSettingsPage(this))
  , mExportDataPage(new ExportDataPage(this))
{
    // Скрываем созданные окна
    mSystemInfoPage     ->hide();
    mZeroSensorPage     ->hide();
    mDateTimePage       ->hide();
    mLanguagePage       ->hide();
    mGeneralSettingsPage->hide();
    mExportDataPage     ->hide();

    // Общий стиль для кнопок
    const QString ToolButtonStyleSheet = readStyleSheetFile(":/styles/ToolButtons.qss");
    // Настраиваем кнопку назад
    setupBackButton(QIcon(":/icons/goBack.svg"), QIcon(":/icons/goBack_pressed.svg"), QSize(70, 83), ToolButtonStyleSheet);

    // Кнопки для верхнего GridLayout
    addSettingButton("Обнуление датчика", QIcon(":/icons/zeroReset.svg"), QIcon(":/icons/zeroReset_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mZeroSensorPage,        0, 0, PageID::ZeroSensor);
    addSettingButton("Язык", QIcon(":/icons/changeLanguage.svg"),                         QIcon(":/icons/changeLanguage_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mLanguagePage,          0, 1, PageID::Language);
    addSettingButton("Время/Дата", QIcon(":/icons/dateTime.svg"),                         QIcon(":/icons/dateTime_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mDateTimePage,          1, 0, PageID::DateTime);
    addSettingButton("Системная информация", QIcon(":/icons/info.svg"),                   QIcon(":/icons/info_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mSystemInfoPage,        1, 1, PageID::SystemInfo);
    addSettingButton("Технический доступ", QIcon(":/icons/tools.svg"),             QIcon(":/icons/tools_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mGeneralSettingsPage,   2, 0, PageID::General);
    addSettingButton("Экспорт данных", QIcon(":/icons/downloadGraph.svg"),                QIcon(":/icons/downloadGraph_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, mExportDataPage,        2, 1, PageID::ExportData);
    addSettingButton("Выключение", QIcon(":/icons/powerOff.svg"),                         QIcon(":/icons/powerOff_pressed.svg"),
                     QSize(125, 125), ToolButtonStyleSheet, nullptr,                3, 0, PageID::PowerOff);

    connect(mSystemInfoPage, &AbstractDialogPage::previousPage, this, &MainMenu::previousPage);
    connect(mZeroSensorPage, &ZeroSensorPage::previousPage, this, &MainMenu::previousPage);
    connect(mDateTimePage,   &DateTimePage::previousPage, this, &MainMenu::previousPage);
    connect(mDateTimePage,   &DateTimePage::changePage, this, &MainMenu::changePage);
    connect(mLanguagePage,   &LanguagePage::previousPage, this, &MainMenu::previousPage);
    connect(mLanguagePage,   &LanguagePage::changePage, this, &MainMenu::changePage);
    connect(mGeneralSettingsPage, &GeneralSettingsPage::previousPage, this, &MainMenu::previousPage);
    connect(mGeneralSettingsPage, &GeneralSettingsPage::changePage, this, &MainMenu::changePage);
    connect(mExportDataPage, &ExportDataPage::previousPage, this, &MainMenu::previousPage);
    connect(mExportDataPage, &ExportDataPage::changePage, this, &MainMenu::changePage);
    connect(mButtonGroup.button(PageID::PowerOff), SIGNAL(clicked()), this, SLOT(powerOff()));
}

MainMenu::~MainMenu()
{
}

void MainMenu::powerOff()
{
    DESTROY_CLASS(mSystemInfoPage);
    DESTROY_CLASS(mZeroSensorPage);
    DESTROY_CLASS(mDateTimePage);
    DESTROY_CLASS(mLanguagePage);
    DESTROY_CLASS(mGeneralSettingsPage);
    DESTROY_CLASS(mExportDataPage);
    mController->terminate();
}

void MainMenu::controllerEventHandler(ControllerEvent event)
{
    switch(event) {
        default: break;
    }
}

void MainMenu::scaleFont(float scaleFactor)
{
    AbstractMenuPage::scaleFont(scaleFactor);
    mSystemInfoPage->scaleFont(scaleFactor);
    mZeroSensorPage->scaleFont(scaleFactor);
    mDateTimePage->scaleFont(scaleFactor);
    mLanguagePage->scaleFont(scaleFactor);
    mGeneralSettingsPage->scaleFont(scaleFactor);
    mExportDataPage->scaleFont(scaleFactor);
}

void MainMenu::installController(MonitorController *controller)
{
    AbstractMenuPage::installController(controller);
    mSystemInfoPage->installController(controller);
    mZeroSensorPage->installController(controller);
    mDateTimePage->installController(controller);
    mLanguagePage->installController(controller);
    mGeneralSettingsPage->installController(controller);
    mExportDataPage->installController(controller);

    connect(mController, &MonitorController::controllerEvent, this , &MainMenu::controllerEventHandler);

    retranslate();
}

void MainMenu::retranslate()
{
    AbstractMenuPage::retranslate();

    mButtonGroup.button(PageID::ZeroSensor)->setText(tr("Обнуление датчика"));
    mButtonGroup.button(PageID::SystemInfo)->setText(tr("Системная информация"));
    mButtonGroup.button(DateTime)  ->setText(tr("Время/Дата"));
    mButtonGroup.button(Language)  ->setText(tr("Язык"));
    mButtonGroup.button(PageID::PowerOff)  ->setText(tr("Выключение"));
    mButtonGroup.button(PageID::General)  ->setText(tr("Технический доступ"));
    mButtonGroup.button(PageID::ExportData)  ->setText(tr("Управление данными"));

    mZeroSensorPage     ->retranslate();
    mSystemInfoPage     ->retranslate();
    mDateTimePage       ->retranslate();
    mLanguagePage       ->retranslate();
    mGeneralSettingsPage->retranslate();
    mExportDataPage     ->retranslate();
}
