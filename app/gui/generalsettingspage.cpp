#include "generalsettingspage.h"
#include "ui_abstractdialogpage.h"

#include "controller/monitorcontroller.h"
#include "controller/settings.h"

#include "gui/dialogWindows/messagedialog.h"
#include "gui_funcs.h"
#include "generalsettingspage.h"
#include "../core/sensor/zsc.h"
#include "../core/sensor/savespi.h"

#include <QString>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

#ifndef PC_BUILD
class ZSC;
extern ZSC mZSC;
#endif

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

bool GeneralSettingsPage::checkInputRegs(QString inputReg)
{
    QRegExp mRegExp("0x([0-9]|[A-F]){4}");
    return mRegExp.exactMatch(inputReg);
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

    if (button->text() == tr("Отключить"))
    {
        qDebug() << "On";


        button->setText(tr("Включить"));
    }
    else
    {
        qDebug() << "Off";
        button->setText(tr("Отключить"));
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

    {
        currentLowState == true ? ui->lowerAlarmSignalButton->setText(tr("Отключить")) : ui->lowerAlarmSignalButton->setText(tr("Включить"));
        currentHighState == true ? ui->upperAlarmSignalButton->setText(tr("Отключить")) : ui->upperAlarmSignalButton->setText(tr("Включить"));
    }
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
    float realDivisionXCount = (float)(settings->getCurrentReadingsGraphIntervalX()) / (settings->getCurrentTickCountX());
    float realDivisionYCount = (float)(settings->getCurrentReadingsGraphIntervalY() - 10) / (settings->getCurrentTickCountY());
    ui->tickCountXLineEdit->setText(QString::number(realDivisionXCount));//settings->getCurrentTickCountX()));
    ui->tickCountYLineEdit->setText(QString::number(realDivisionYCount));//settings->getCurrentTickCountY()));
    ui->pressureUnitsComboBox->setCurrentIndex(settings->getCurrentPressureIndex());
    ui->averageIntervalSecLineEdit->setText(QString::number(settings->getCurrentAverageIntervalSec()));
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


#define setRegs(reg, num) reg->setText("0x" + QString::number(settings->mRegValues[num], 16).rightJustified(4, '0').toUpper())
    for (uint8_t i=0; i<32; i++)
    {
        QLineEdit *find = ui->regsTableWidget->findChild<QLineEdit*>("reg" + QString::number(i));
        setRegs(find, i);
    }
}

//#define indexPressureH2O 13.595
void GeneralSettingsPage::updateParameters()
{
    if (!mController) {
        return;
    }
    bool isPressureUnitsChanged = false;
    qDebug() << "FromisPressureUnitsChanged" << isPressureUnitsChanged;
    //mController


    mLineEditList = ui->scrollAreaWidgetContents->findChildren<QLineEdit*>();//scrollArea->findChildren<QLineEdit*>();
    for (int i = 0; i < mLineEditList.count(); i++)
    {
        if (mLineEditList[i]->isReadOnly())
        {
            continue;
        }
        if (mLineEditList[i]->parent() == ui->regsTableWidget)
        {
            if (!checkInputRegs(mLineEditList[i]->text()))
            {
                openSettingsInfoErrorDialog(tr("Введены некорректные значения регистров"));
                return;
            }
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
    float mCurrentReadingsGraphIntervalX = ui->intervalXLineEdit->text().toFloat();
    float mCurrentReadingsGraphIntervalY = ui->intervalYLineEdit->text().toFloat();
    float mTickCountX = (float)mCurrentReadingsGraphIntervalX / ui->tickCountXLineEdit->text().toFloat();
    float mTickCountY = (float)(mCurrentReadingsGraphIntervalY - 10) / ui->tickCountYLineEdit->text().toFloat();
    float mHighLevelAlarm = ui->upperAlarmLineEdit->text().toFloat();
    float mLowLevelAlarm = ui->lowerAlarmLineEdit->text().toFloat();
    float mAverageIntervalSec = ui->averageIntervalSecLineEdit->text().toFloat();

    bool mLowLevelStateAlarm = true;
    bool mHighLevelStateAlarm = true;
    if (ui->lowerAlarmSignalButton->text() == tr("Включить")) { mLowLevelStateAlarm = false; }
    if (ui->upperAlarmSignalButton->text() == tr("Включить")) { mHighLevelStateAlarm = false; }

    if (isPressureUnitsChanged == true)
    {
        if (mPressureUnitsIndex == 0) // 1 -> 0
        {
            mCurrentReadingsGraphIntervalY = (round)(mCurrentReadingsGraphIntervalY / indexPressureH2O);
            //mTickCountY /= indexPressureH2O;
            mHighLevelAlarm = (round)(mHighLevelAlarm / indexPressureH2O);
            mLowLevelAlarm = (round)(mLowLevelAlarm / indexPressureH2O);
            mCurrentMaxYRange /= indexPressureH2O;
        }
        else // 0 -> 1
        {
            mCurrentReadingsGraphIntervalY = (round)(mCurrentReadingsGraphIntervalY * indexPressureH2O);
            //mTickCountY *= indexPressureH2O;
            mHighLevelAlarm = (round)(mHighLevelAlarm * indexPressureH2O);
            mLowLevelAlarm = (round)(mLowLevelAlarm * indexPressureH2O);
            mCurrentMaxYRange *= indexPressureH2O;
        }
        qDebug() << "mCurrentReadingsGraphIntervalY" <<mCurrentReadingsGraphIntervalY;
        qDebug() << "mTickCountY" << mTickCountY;
        qDebug() << "mHighLevelAlarm" << mHighLevelAlarm;
        qDebug() << "mLowLevelAlarm" << mLowLevelAlarm;
        qDebug() << "mCurrentMaxYRange" << mCurrentMaxYRange;

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
    \
    QTimer::singleShot(0, mController, [this, mAverageIntervalSec]()
    {
        if (mController->setAverageInterval(mAverageIntervalSec))
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


    //обновление регистров после подтверждения
    uint16_t tempRegs[32];
    for (uint8_t i=0; i<32; i++)
    {
        QString ttttt = "reg" + QString::number(i);
        QLineEdit *find = ui->regsTableWidget->findChild<QLineEdit*>(ttttt);
        tempRegs[i] = find->text().split("0x")[1].toInt(nullptr, 16);
        qDebug() << tempRegs[i];
    }
    mICPSettings->setRegsValues(tempRegs);
    #ifndef PC_BUILD
    mZSC.resetRegsValues();
    #endif


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
    WFontScaling(ui->pressureUnitsLabel, scaleFactor);
    WFontScaling(ui->pressureUnitsComboBox, scaleFactor);
    WFontScaling(ui->averageIntervalSecLabel, scaleFactor);
    WFontScaling(ui->averageIntervalSecLineEdit, scaleFactor);

    WFontScaling(ui->registersTitle, scaleFactor);
    WFontScaling(ui->column0, scaleFactor);
    WFontScaling(ui->column1, scaleFactor);
    WFontScaling(ui->column2, scaleFactor);
    WFontScaling(ui->column3, scaleFactor);
    WFontScaling(ui->row0, scaleFactor);
    WFontScaling(ui->row1, scaleFactor);
    WFontScaling(ui->row2, scaleFactor);
    WFontScaling(ui->row3, scaleFactor);
    WFontScaling(ui->row4, scaleFactor);
    WFontScaling(ui->row5, scaleFactor);
    WFontScaling(ui->row6, scaleFactor);
    WFontScaling(ui->row7, scaleFactor);

    WFontScaling(ui->reg0, scaleFactor);
    WFontScaling(ui->reg1, scaleFactor);
    WFontScaling(ui->reg2, scaleFactor);
    WFontScaling(ui->reg3, scaleFactor);
    WFontScaling(ui->reg4, scaleFactor);
    WFontScaling(ui->reg5, scaleFactor);
    WFontScaling(ui->reg6, scaleFactor);
    WFontScaling(ui->reg7, scaleFactor);
    WFontScaling(ui->reg8, scaleFactor);
    WFontScaling(ui->reg9, scaleFactor);
    WFontScaling(ui->reg10, scaleFactor);
    WFontScaling(ui->reg11, scaleFactor);
    WFontScaling(ui->reg12, scaleFactor);
    WFontScaling(ui->reg13, scaleFactor);
    WFontScaling(ui->reg14, scaleFactor);
    WFontScaling(ui->reg15, scaleFactor);
    WFontScaling(ui->reg16, scaleFactor);
    WFontScaling(ui->reg17, scaleFactor);
    WFontScaling(ui->reg18, scaleFactor);
    WFontScaling(ui->reg19, scaleFactor);
    WFontScaling(ui->reg20, scaleFactor);
    WFontScaling(ui->reg21, scaleFactor);
    WFontScaling(ui->reg22, scaleFactor);
    WFontScaling(ui->reg23, scaleFactor);
    WFontScaling(ui->reg24, scaleFactor);
    WFontScaling(ui->reg25, scaleFactor);
    WFontScaling(ui->reg26, scaleFactor);
    WFontScaling(ui->reg27, scaleFactor);
    WFontScaling(ui->reg28, scaleFactor);
    WFontScaling(ui->reg29, scaleFactor);
    WFontScaling(ui->reg30, scaleFactor);
    WFontScaling(ui->reg31, scaleFactor);

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

