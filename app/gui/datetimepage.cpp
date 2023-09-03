#include "datetimepage.h"
#include "ui_datetimepage.h"
#include "ui_abstractdialogpage.h"
#include "controller/monitorcontroller.h"
#include "gui/gui_funcs.h"

#include <QDateTimeEdit>
#include <QDialog>
#include <QDebug>
#include <QTimer>

DateTimePage::DateTimePage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::DateTimePage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);

    // Установка картинки окна
    //setUpperSvgIcon(":/icons/alarm.svg");

    QDateTimeEdit *dateEditSB = new QDateTimeEdit(ui->inputDateSpinBox);
    dateEditSB->setMaximumDate(QDate(2038, 1, 18));
    dateEditSB->setMinimumDate(QDate(2000, 1, 1));
    dateEditSB->setMinimumWidth(300);
    dateEditSB->setDisplayFormat("dd.MM.yyyy");

    QDateTimeEdit *timeEditSB = new QDateTimeEdit(ui->inputTimeSpinBox);
    timeEditSB->setMinimumWidth(300);
    timeEditSB->setDisplayFormat("hh:mm");

    ui->inputDateSpinBox->setSpinBox(dateEditSB);
    ui->inputTimeSpinBox->setSpinBox(timeEditSB);
}

DateTimePage::~DateTimePage()
{
    delete ui;
}

void DateTimePage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Настройка даты и времени"));
    setBottomInfoLabel("");
}

void DateTimePage::showEvent(QShowEvent *)
{
    auto dateTime = QDateTime::currentDateTime();
    ui->inputTimeSpinBox->setTime(dateTime.time());
    ui->inputDateSpinBox->setDate(dateTime.date());
}

void DateTimePage::done(int exodus)
{
    if (exodus == QDialog::Accepted && mController) {
        bool okTime = false;
        bool okDate = false;
        auto ti = ui->inputTimeSpinBox->time(&okTime);
        auto da = ui->inputDateSpinBox->date(&okDate);

        if (okTime && okDate) {
            ti.setHMS(ti.hour(), ti.minute(), 0); // Обнуляем секунды
            QTimer::singleShot(0, mController, [this, da, ti]() { mController->setDateTime(QDateTime(da, ti).toSecsSinceEpoch()); });
        }
    }
    emit previousPage();
}

void DateTimePage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);
    retranslate();
}

void DateTimePage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);
    WFontScaling(ui->inputTimeSpinBox, scaleFactor);
    WFontScaling(ui->inputDateSpinBox, scaleFactor);
    WFontScaling(ui->timeLabel, scaleFactor);
    WFontScaling(ui->dateLabel, scaleFactor);
}
