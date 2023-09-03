#include "settingsmenu.h"
#include "ui_abstractmenupage.h"
#include "gui/datetimepage.h"
#include "gui/languagepage.h"
#include "gui/gui_funcs.h"
#include "gui/technicalaccesspage.h"
#include "controller/monitorcontroller.h"

#include <QAbstractButton>

SettingsMenu::SettingsMenu(QWidget *parent) :
    AbstractMenuPage(parent),
    mDateTimePage(new DateTimePage(this)),
    mLanguagePage(new LanguagePage(this)),
    mTechnicalAccessPage(new TechnicalAccessPage(this))
{
    // Скрываем созданные окна
    mDateTimePage->hide();
    mLanguagePage->hide();
    mTechnicalAccessPage->hide();

    // Общий стиль для кнопок
    const QString ToolButtonStyleSheet = readStyleSheetFile(":/styles/ToolButtons.qss");

    // Настройка анимации кнопки назад
    setupBackButton(QIcon(":/icons/home.svg"), QIcon(":/icons/home_pressed.svg"), QSize(125, 125), ToolButtonStyleSheet);

    // Кнопки для верхнего GridLayout
    addSettingButton("Время/Дата", QIcon(":/icons/dateTime.svg"), QIcon(":/icons/dateTime_pressed.svg"), QSize(125, 125), ToolButtonStyleSheet, mDateTimePage, 0, 0, IDDateTimeButton);
    addSettingButton("Язык", QIcon(":/icons/changeLanguage.svg"), QIcon(":/icons/changeLanguage_pressed.svg"), QSize(125, 125), ToolButtonStyleSheet, mLanguagePage, 1, 0, IDLanguageButton);
    addSettingButton("Технический доступ", QIcon(":/icons/zeroReset.svg"), QIcon(":/icons/zeroReset_pressed.svg"), QSize(125, 125), ToolButtonStyleSheet, mTechnicalAccessPage, 0, 1, IDTechnicalAccessButton);

    // Смена виджета на предыдущий
    connect(mDateTimePage, &IPageWidget::previousPage, this, &IPageWidget::previousPage);
    connect(mLanguagePage, &IPageWidget::previousPage, this, &IPageWidget::previousPage);
    connect(mTechnicalAccessPage, &IPageWidget::previousPage, this, &IPageWidget::previousPage);
}

void SettingsMenu::scaleFont(float scaleFactor)
{
    AbstractMenuPage::scaleFont(scaleFactor);
    mDateTimePage->scaleFont(scaleFactor);
    mLanguagePage->scaleFont(scaleFactor);
    mTechnicalAccessPage->scaleFont(scaleFactor);
}

void SettingsMenu::installController(MonitorController *controller)
{
    AbstractMenuPage::installController(controller);
    mDateTimePage->installController(controller);
    mLanguagePage->installController(controller);
    mTechnicalAccessPage->installController(controller);

    retranslate();
}

void SettingsMenu::retranslate()
{
    AbstractMenuPage::retranslate();

    mButtonGroup.button(IDDateTimeButton)->setText(tr("Время/Дата"));
    mButtonGroup.button(IDLanguageButton)->setText(tr("Язык"));
    mButtonGroup.button(IDTechnicalAccessButton)->setText(tr("Технический доступ"));

    mDateTimePage->retranslate();
    mLanguagePage->retranslate();
    mTechnicalAccessPage->retranslate();
}

void SettingsMenu::showEvent(QShowEvent */*event*/)
{
    // Если контроллера нет, то запрещаем изменять время
    if (!mController) {
        mButtonGroup.button(IDDateTimeButton)->setDisabled(true);
        return;
    }

    // Если сессия есть, то запрещаем изменять время
    if (const auto &session = mController->getCurrentSession()) {
        mButtonGroup.button(IDDateTimeButton)->setDisabled(true);
    } else {
        mButtonGroup.button(IDDateTimeButton)->setDisabled(false);
    }
}
