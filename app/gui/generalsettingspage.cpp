#include "generalsettingspage.h"
#include "ui_abstractdialogpage.h"

#include "controller/monitorcontroller.h"
#include "controller/settings.h"

#include "gui/dialogWindows/messagedialog.h"
#include "gui_funcs.h"

#include <QString>
#include <QTimer>
#include <QDebug>

GeneralSettingsPage::GeneralSettingsPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::GeneralSettingsPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);

    // Настройка окна сообщений
    setupMessageBox();

    setupAlarm();
}

GeneralSettingsPage::~GeneralSettingsPage()
{
    delete ui;
}

bool GeneralSettingsPage::checkInputData(QString inputParametr)
{
    QRegExp mRegExp("(0|[1-9][\\d]*)(\\.[\\d]+)?");
    return mRegExp.exactMatch(inputParametr);
}

void GeneralSettingsPage::setupAlarm()
{
    connect(ui->lowerAlarmSignalButton, &QPushButton::clicked, this, &GeneralSettingsPage::changeAlarmStatus);
    connect(ui->upperAlarmSignalButton, &QPushButton::clicked, this, &GeneralSettingsPage::changeAlarmStatus);
}


void GeneralSettingsPage::changeAlarmStatus()
{
    QPushButton* button = (QPushButton*)(QObject::sender());
    qDebug() << button;
    if (button->text() == "Отключить")
    {
        qDebug() << "On";
        button->setText("Включить");
    }
    else
    {
        qDebug() << "Off";
        button->setText("Отключить");
    }
}

void GeneralSettingsPage::setupMessageBox()
{
    if (mMessageDialog) {
        return;
    }
    mMessageDialog = new MessageDialog(this);
}

void GeneralSettingsPage::openSettingsInfoErrorDialog(const QString &info)
{
    mMessageDialog->setTextMessage(info);
    mMessageDialog->open();
}

void GeneralSettingsPage::updateAlarmSettingsOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    ui->lowerAlarmLineEdit->setText(QString::number(settings->getLowLevelAlarm()));
    ui->upperAlarmLineEdit->setText(QString::number(settings->getHighLevelAlarm()));

    const bool currentLowState = settings->getLowLevelStateAlarm();
    const bool currentHighState = settings->getHighLevelStateAlarm();


    currentLowState == true ? ui->lowerAlarmSignalButton->setText("Отключить") : ui->lowerAlarmSignalButton->setText("Включить");
    currentHighState == true ? ui->upperAlarmSignalButton->setText("Отключить") : ui->upperAlarmSignalButton->setText("Включить");

}


void GeneralSettingsPage::updateGraphSettingsOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    ui->intervalXLineEdit->setText(QString::number(settings->getCurrentReadingsGraphIntervalX()));
    ui->intervalYLineEdit->setText(QString::number(settings->getCurrentReadingsGraphIntervalY()));
    ui->relativeCurrentPathLineEdit->setText(settings->getRelativeCurrentSensorReadingsPath());
    ui->currentMaxStorageTimeLineEdit->setText(QString::number(settings->getMaxTimeStorageCurrentSensorReadingsMs()));
    double realDivisionXCount = (double)(settings->getCurrentReadingsGraphIntervalX()) / (settings->getCurrentTickCountX());
    double realDivisionYCount = (double)(settings->getCurrentReadingsGraphIntervalY() - 10) / (settings->getCurrentTickCountY());
    ui->tickCountXLineEdit->setText(QString::number(realDivisionXCount));//settings->getCurrentTickCountX()));
    ui->tickCountYLineEdit->setText(QString::number(realDivisionYCount));//settings->getCurrentTickCountY()));
    ui->pressureUnitsComboBox->setCurrentIndex(settings->getCurrentPressureIndex());
}

void GeneralSettingsPage::updateGeneralSettingsOnWidgets()
{
    if (!mController) {
        return;
    }

    const Settings * const settings = mController->settings();
    if (!settings) {
        return;
    }

    ui->fontScaleFactorLineEdit->setText(QString::number(settings->getFontScaleFactor()));
    ui->softwareStorageUUIDLineEdit->setText(settings->getSoftwareStorageUUID());
}

#define indexPressureH2O 13.595
void GeneralSettingsPage::updateParameters()
{
    if (!mController) {
        return;
    }
    bool isPressureUnitsChanged = false;
    qDebug() << "FromisPressureUnitsChanged" << isPressureUnitsChanged;
    //mController
    mLineEditList = ui->scrollArea->findChildren<QLineEdit*>();
    for (int i = 0; i < mLineEditList.count(); i++)
    {
        if (mLineEditList[i]->isReadOnly())
        {
            continue;
        }
        if (!checkInputData(mLineEditList[i]->text()))
        {
            openSettingsInfoErrorDialog(tr("Введены некорректные параметры"));
            return;
        }
    }
    uint8_t mPressureUnitsIndex = ui->pressureUnitsComboBox->currentIndex();
    //QTimer::singleShot(0, mController, [this, mPressureUnitsIndex, &isPressureUnitsChanged]()
    {
        if (mController->setPressureUnits(mPressureUnitsIndex))
        {
            //emit previousPage();
            isPressureUnitsChanged = true;
            //qDebug() << "ToisPressureUnitsChanged" << isPressureUnitsChanged;
        }
        emit previousPage();
//        else { openAlarmInfoErrorDialog(tr("Верхний уровень должен\nбыть больше, чем нижний!"));}S
    }//);
qDebug() << "ToisPressureUnitsChanged" << isPressureUnitsChanged;
    double mCurrentReadingsGraphIntervalX = ui->intervalXLineEdit->text().toDouble();
    double mCurrentReadingsGraphIntervalY = ui->intervalYLineEdit->text().toDouble();
    double mTickCountX = (double)mCurrentReadingsGraphIntervalX / ui->tickCountXLineEdit->text().toDouble();
    double mTickCountY = (double)(mCurrentReadingsGraphIntervalY - 10) / ui->tickCountYLineEdit->text().toDouble();
    double mHighLevelAlarm = ui->upperAlarmLineEdit->text().toFloat();
    double mLowLevelAlarm = ui->lowerAlarmLineEdit->text().toFloat();

    bool mLowLevelStateAlarm = true;
    bool mHighLevelStateAlarm = true;
    if (ui->lowerAlarmSignalButton->text() == "Включить") { mLowLevelStateAlarm = false; }
    if (ui->upperAlarmSignalButton->text() == "Включить") { mHighLevelStateAlarm = false; }

    if (isPressureUnitsChanged == true)
    {
        if (mPressureUnitsIndex == 0) // 1 -> 0
        {
            mCurrentReadingsGraphIntervalY /= indexPressureH2O;
            mTickCountY /= indexPressureH2O;
            mHighLevelAlarm /= indexPressureH2O;
            mLowLevelAlarm /= indexPressureH2O;
        }
        else // 0 -> 1
        {
            mCurrentReadingsGraphIntervalY *= indexPressureH2O;
            mTickCountY *= indexPressureH2O;
            mHighLevelAlarm *= indexPressureH2O;
            mLowLevelAlarm *= indexPressureH2O;
        }
        qDebug() << "mCurrentReadingsGraphIntervalY" <<mCurrentReadingsGraphIntervalY;
        qDebug() << "mTickCountY" << mTickCountY;
        qDebug() << "mHighLevelAlarm" << mHighLevelAlarm;
        qDebug() << "mLowLevelAlarm" << mLowLevelAlarm;

        mController->setInetrvalsOnGraph(mCurrentReadingsGraphIntervalX, mCurrentReadingsGraphIntervalY,
                                                     mTickCountX, mTickCountY);
        mController->setLevelsAndStatesAlarm(mLowLevelAlarm, mHighLevelAlarm, mLowLevelStateAlarm, mHighLevelStateAlarm);
        mICPSettings->setAllPressureParam(mCurrentReadingsGraphIntervalY, mTickCountY,
                                          mHighLevelAlarm, mLowLevelAlarm);
        updateAlarmSettingsOnWidgets();
        updateGraphSettingsOnWidgets();
}


    float mFontScaleFactor = ui->fontScaleFactorLineEdit->text().toFloat();
    QString mSoftwareStorageUUID = ui->softwareStorageUUIDLineEdit->text();

    // Устанавливаем
    QTimer::singleShot(0, mController, [this, mLowLevelAlarm, mHighLevelAlarm, mLowLevelStateAlarm, mHighLevelStateAlarm]()
    {
        if (mController->setLevelsAndStatesAlarm(mLowLevelAlarm, mHighLevelAlarm, mLowLevelStateAlarm, mHighLevelStateAlarm))
        {
            emit previousPage();
        }
//        else { openAlarmInfoErrorDialog(tr("Верхний уровень должен\nбыть больше, чем нижний!"));}
    });

    QTimer::singleShot(0, mController, [this, mCurrentReadingsGraphIntervalX,
                       mCurrentReadingsGraphIntervalY, mTickCountX, mTickCountY]()
    {
        if (mController->setInetrvalsOnGraph(mCurrentReadingsGraphIntervalX, mCurrentReadingsGraphIntervalY,
                                             mTickCountX, mTickCountY))
        {
            emit previousPage();
        }
//        else { openAlarmInfoErrorDialog(tr("Верхний уровень должен\nбыть больше, чем нижний!"));}
    });

    QTimer::singleShot(0, mController, [this, mFontScaleFactor]()
    {
        if (mController->setGeneralParam(mFontScaleFactor))
        {
            emit previousPage();
        }
//        else { openAlarmInfoErrorDialog(tr("Верхний уровень должен\nбыть больше, чем нижний!"));}
    });

}

void GeneralSettingsPage::done(int exodus)
{
    if (exodus != QDialog::Accepted) {
        emit previousPage();
        return;
    }

    // Обновление всех параметров
    updateParameters();
}

void GeneralSettingsPage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);
    WFontScaling(ui->intervalXLabel, scaleFactor);
    WFontScaling(ui->intervalYLabel, scaleFactor);
    WFontScaling(ui->intervalXLineEdit, scaleFactor);
    WFontScaling(ui->intervalYLineEdit, scaleFactor);
    WFontScaling(ui->tickCountXLabel, scaleFactor);
    WFontScaling(ui->tickCountXLineEdit, scaleFactor);
    WFontScaling(ui->tickCountYLabel, scaleFactor);
    WFontScaling(ui->tickCountYLineEdit, scaleFactor);
    WFontScaling(ui->upperAlarmLabel, scaleFactor);
    WFontScaling(ui->upperAlarmLineEdit, scaleFactor);
    WFontScaling(ui->upperAlarmSignalLabel, scaleFactor);
    WFontScaling(ui->upperAlarmSignalButton, scaleFactor);
    WFontScaling(ui->lowerAlarmLabel, scaleFactor);
    WFontScaling(ui->lowerAlarmLineEdit, scaleFactor);
    WFontScaling(ui->lowerAlarmSignalLabel, scaleFactor);
    WFontScaling(ui->lowerAlarmSignalButton, scaleFactor);
    WFontScaling(ui->currentMaxStorageTimeLineEdit, scaleFactor);
    WFontScaling(ui->currentMaxStorageTimeLabel, scaleFactor);
    WFontScaling(ui->relativeCurrentPathLabel, scaleFactor);
    WFontScaling(ui->relativeCurrentPathLineEdit, scaleFactor);
    WFontScaling(ui->fontScaleFactorLabel, scaleFactor);
    WFontScaling(ui->fontScaleFactorLineEdit, scaleFactor);
    WFontScaling(ui->softwareStorageUUIDLabel, scaleFactor);
    WFontScaling(ui->softwareStorageUUIDLineEdit, scaleFactor);

    mMessageDialog->scaleFont(scaleFactor);
}

void GeneralSettingsPage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);
    retranslate();
}

void GeneralSettingsPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Технический доступ"));
    setBottomInfoLabel(tr(""));

    mMessageDialog->retranslate();
}

void GeneralSettingsPage::showEvent(QShowEvent */*event*/)
{
    updateAlarmSettingsOnWidgets();
    updateGraphSettingsOnWidgets();
    updateGeneralSettingsOnWidgets();
}

