#include "datetimepage.h"
#include "ui_datetimepage.h"
#include "ui_abstractdialogpage.h"
#include "controller/monitorcontroller.h"
#include "gui/gui_funcs.h"
#include "clock.h"

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
#ifdef PC_BUILD
    float coeff = 0.6;
    WFontScaling(ui->inputTimeSpinBox, coeff);
    WFontScaling(ui->inputDateSpinBox, coeff);
    WFontScaling(ui->timeLabel, coeff);
    WFontScaling(ui->dateLabel, coeff);
#endif
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

typedef struct
{
    uint8_t sec;   // 000
    uint8_t min;
    uint8_t hour;
    uint8_t ofday;
    uint8_t day;
    uint8_t month;
    uint8_t years;
}_currDate;


void DateTimePage::done(int exodus)
{
    uint8_t mCurrDateTime[7];
    if (exodus == QDialog::Accepted && mController) {
        bool okTime = false;
        bool okDate = false;
        auto ti = ui->inputTimeSpinBox->time(&okTime);
        auto da = ui->inputDateSpinBox->date(&okDate);

        QString ttt = ti.toString();
        qDebug() << ttt;
        mCurrDateTime[3] = 1;
        mCurrDateTime[2] = ((ttt[0].toLatin1() - 0x30) << 4) + ttt[1].toLatin1() - 0x30;
        mCurrDateTime[1] = ((ttt[3].toLatin1() - 0x30) << 4) + ttt[4].toLatin1() - 0x30;
        mCurrDateTime[0] = 0;

        ttt = da.toString("yy.MM.dd");
        qDebug() << ttt;
        mCurrDateTime[6] = ((ttt[0].toLatin1() - 0x30) << 4) + ttt[1].toLatin1() - 0x30;
        mCurrDateTime[5] = ((ttt[3].toLatin1() - 0x30) << 4) + ttt[4].toLatin1() - 0x30;
        mCurrDateTime[4] = ((ttt[6].toLatin1() - 0x30) << 4) + ttt[7].toLatin1() - 0x30;
        qDebug() << mCurrDateTime[6] << mCurrDateTime[5]  << mCurrDateTime[4] << mCurrDateTime[2] << mCurrDateTime[1];
#ifdef TEST_BUILD
        setRTC(mCurrDateTime);
#else
        setDateTime(mCurrDateTime);
        qDebug() << QDateTime::currentDateTime();
#endif
        ui->inputTimeSpinBox->setTime(QTime::currentTime());// dateTime.time());
        ui->inputDateSpinBox->setDate(QDate::currentDate());// dateTime.date());
        //getRTC(mCurrDateTime);
//        if (okTime && okDate) {
//            ti.setHMS(ti.hour(), ti.minute(), 0); // Обнуляем секунды
//            QTimer::singleShot(0, mController, [this, da, ti]() { mController->setDateTime(QDateTime(da, ti).toSecsSinceEpoch()); });
//        }
        mController->controllerEvent(ControllerEvent::GlobalTimeUpdate);
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
