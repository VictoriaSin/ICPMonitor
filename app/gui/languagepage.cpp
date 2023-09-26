#include "languagepage.h"
#include "ui_languagepage.h"
#include "ui_abstractdialogpage.h"
#include "controller/monitorcontroller.h"
#include "controller/settings.h"
#include "gui_funcs.h"
#include "../app/global_define.h"

#include <QDialog>
#include <QTimer>
#include <QDebug>

LanguagePage::LanguagePage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::LanguagePage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);
}

void LanguagePage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Выбор языка"));
    setBottomInfoLabel(tr("Для смены языка необходимо\n"
                          "подтвердить ваш выбор!"));
}

LanguagePage::~LanguagePage()
{
    delete ui;
}

void LanguagePage::updateCurrentAppLanguageOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    switch (settings->getCurrentAppLanguage()) {
    case QLocale::Language::Russian: {
        ui->rusLangRB->setChecked(true);
        break;
    }
    case QLocale::Language::English: {
        ui->engLangRB->setChecked(true);
        break;
    }
    default: break;
    }
}

void LanguagePage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);
    retranslate();
}

void LanguagePage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);
    WFontScaling(ui->rusLangRB, scaleFactor);
    WFontScaling(ui->engLangRB, scaleFactor);
}

void LanguagePage::done(int exodus)
{
    // Если пользователь отклонил изменения
    if (exodus != QDialog::Accepted) {
        emit previousPage();
        return;
    }

    // Установка языка приложения, выбранного пользователем
    if (ui->rusLangRB->isChecked())
    {
        QTimer::singleShot(0, mController, [this]() {
            mController->setAppLanguage(QLocale::Language::Russian);
        });
    }
    else if (ui->engLangRB->isChecked())
    {
        QTimer::singleShot(0, mController, [this]() {
            mController->setAppLanguage(QLocale::Language::English);
        });
    }
    //mICPSettings->writeAllSetting();
    emit previousPage();
}

void LanguagePage::showEvent(QShowEvent */*event*/)
{
    updateCurrentAppLanguageOnWidgets();
}
