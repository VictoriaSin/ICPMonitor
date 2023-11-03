#include "generalsettingspage.h"
#include "ui_abstractdialogpage.h"

#include "controller/monitorcontroller.h"
#include "controller/settings.h"

#include "gui/dialogWindows/messagedialog.h"
#include "gui_funcs.h"
#include "generalsettingspage.h"
#include "../core/sensor/zsc.h"
#include "../core/sensor/read_spi.h"

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

    ui->aCoefficientLineEdit->setObjectName("coefficientA");
    ui->bCoefficientLineEdit->setObjectName("coefficientB");

#ifdef PC_BUILD
    float coeff = 0.7;

    WFontScaling(ui->intervalXLabel,                coeff);
    WFontScaling(ui->lowIntervalYLabel,             coeff);
    WFontScaling(ui->highIntervalYLabel,            coeff);
    WFontScaling(ui->intervalXLineEdit,             coeff);
    WFontScaling(ui->lowIntervalYLineEdit,          coeff);
    WFontScaling(ui->highIntervalYLineEdit,         coeff);
    WFontScaling(ui->tickCountXLabel,               coeff);
    WFontScaling(ui->tickCountXLineEdit,            coeff);
    WFontScaling(ui->tickCountYLabel,               coeff);
    WFontScaling(ui->tickCountYLineEdit,            coeff);
    WFontScaling(ui->upperAlarmLabel,               coeff);
    WFontScaling(ui->upperAlarmLineEdit,            coeff);
    WFontScaling(ui->upperAlarmSignalLabel,         coeff);
    WFontScaling(ui->upperAlarmSignalButton,        coeff);
    WFontScaling(ui->lowerAlarmLabel,               coeff);
    WFontScaling(ui->lowerAlarmLineEdit,            coeff);
    WFontScaling(ui->lowerAlarmSignalLabel,         coeff);
    WFontScaling(ui->lowerAlarmSignalButton,        coeff);
    WFontScaling(ui->currentMaxStorageTimeLineEdit, coeff);
    WFontScaling(ui->currentMaxStorageTimeLabel,    coeff);
    WFontScaling(ui->relativeCurrentPathLabel,      coeff);
    WFontScaling(ui->relativeCurrentPathLineEdit,   coeff);
    WFontScaling(ui->fontScaleFactorLabel,          coeff);
    WFontScaling(ui->fontScaleFactorLineEdit,       coeff);
    WFontScaling(ui->softwareStorageUUIDLabel,      coeff);
    WFontScaling(ui->softwareStorageUUIDLineEdit,   coeff);
    WFontScaling(ui->pressureUnitsLabel,            coeff);
    WFontScaling(ui->pressureUnitsComboBox,         coeff);
    WFontScaling(ui->averageIntervalSecLabel,       coeff);
    WFontScaling(ui->averageIntervalSecLineEdit,    coeff);
    WFontScaling(ui->aCoefficientLabel,             coeff);
    WFontScaling(ui->aCoefficientLineEdit,          coeff);
    WFontScaling(ui->bCoefficientLabel,             coeff);
    WFontScaling(ui->bCoefficientLineEdit,          coeff);

    WFontScaling(ui->registersTitle, coeff);
    WFontScaling(ui->column0,        coeff);
    WFontScaling(ui->column1,        coeff);
    WFontScaling(ui->column2,        coeff);
    WFontScaling(ui->column3,        coeff);
    WFontScaling(ui->row0,           coeff);
    WFontScaling(ui->row1,           coeff);
    WFontScaling(ui->row2,           coeff);
    WFontScaling(ui->row3,           coeff);
    WFontScaling(ui->row4,           coeff);
    WFontScaling(ui->row5,           coeff);
    WFontScaling(ui->row6,           coeff);
    WFontScaling(ui->row7,           coeff);

    mMessageDialog->scaleFont(coeff);
#endif
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

bool GeneralSettingsPage::checkInputCoeff(QString inputCoeff)
{
    QRegExp mRegExp("(-)?(0|[1-9][\\d]*)(\\.[\\d]+)?");
    return mRegExp.exactMatch(inputCoeff);
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
    ui->lowIntervalYLineEdit->setText(QString::number(settings->getCurrentReadingsGraphIntervalYLow()));
    ui->highIntervalYLineEdit->setText(QString::number(settings->getCurrentReadingsGraphIntervalYHigh()));
    ui->relativeCurrentPathLineEdit->setText(settings->getRelativeCurrentSensorReadingsPath());
    ui->currentMaxStorageTimeLineEdit->setText(QString::number(settings->getMaxTimeStorageCurrentSensorReadingsMs()));
    float realDivisionXCount = (float)(settings->getCurrentReadingsGraphIntervalX()) / (settings->getCurrentTickCountX());
    float realDivisionYCount = (float)(settings->getCurrentReadingsGraphIntervalYHigh() - settings->getCurrentReadingsGraphIntervalYLow()) / (settings->getCurrentTickCountY()); // 10
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
    ui->aCoefficientLineEdit->setText(QString::number(settings->getACoefficient()));
    ui->bCoefficientLineEdit->setText(QString::number(settings->getBCoefficient()));

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
        if (mLineEditList[i]->objectName() == "coefficientA" || mLineEditList[i]->objectName() == "coefficientB")
        {
            if (!checkInputCoeff(mLineEditList[i]->text()))
            {
                openSettingsInfoErrorDialog(tr("Введены некорректные коэффициенты a, b"));
                return;
            }
        }
        else if (!checkInputData(mLineEditList[i]->text()))
        {
            openSettingsInfoErrorDialog(tr("Введены некорректные параметры"));
            return;
        }
    }

// временно
    if (mICPSettings->getACoefficient() != ui->aCoefficientLineEdit->text().toFloat()
        || mICPSettings->getBCoefficient() != ui->bCoefficientLineEdit->text().toFloat())
    {
        mICPSettings->setACoefficient(ui->aCoefficientLineEdit->text().toFloat());
        mICPSettings->setBCoefficient(ui->bCoefficientLineEdit->text().toFloat());
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
    float mCurrentReadingsGraphIntervalYLow = ui->lowIntervalYLineEdit->text().toFloat();
    float mCurrentReadingsGraphIntervalYHigh = ui->highIntervalYLineEdit->text().toFloat();

    if (ui->tickCountXLineEdit->text().toFloat() < 0.1 || ui->tickCountYLineEdit->text().toFloat() < 0.1)
    {
        openSettingsInfoErrorDialog(tr("Введен некорректный шаг\nделений оси"));
        return;
    }
    float mTickCountX = (float)mCurrentReadingsGraphIntervalX / ui->tickCountXLineEdit->text().toFloat();
    float mTickCountY = (float)(mCurrentReadingsGraphIntervalYHigh - mCurrentReadingsGraphIntervalYLow) / ui->tickCountYLineEdit->text().toFloat(); // 10
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
            mCurrentReadingsGraphIntervalYLow = (round)(mCurrentReadingsGraphIntervalYLow / indexPressureH2O);
            mCurrentReadingsGraphIntervalYHigh = (round)(mCurrentReadingsGraphIntervalYHigh / indexPressureH2O);
            //mTickCountY /= indexPressureH2O;
            mHighLevelAlarm = (round)(mHighLevelAlarm / indexPressureH2O);
            mLowLevelAlarm = (round)(mLowLevelAlarm / indexPressureH2O);
            mCurrentMaxYRange /= indexPressureH2O;
        }
        else // 0 -> 1
        {
            mCurrentReadingsGraphIntervalYLow = (round)(mCurrentReadingsGraphIntervalYLow * indexPressureH2O);
            mCurrentReadingsGraphIntervalYHigh = (round)(mCurrentReadingsGraphIntervalYHigh * indexPressureH2O);
            //mTickCountY *= indexPressureH2O;
            mHighLevelAlarm = (round)(mHighLevelAlarm * indexPressureH2O);
            mLowLevelAlarm = (round)(mLowLevelAlarm * indexPressureH2O);
            mCurrentMaxYRange *= indexPressureH2O;
        }

        mController->setInetrvalsOnGraph(mCurrentReadingsGraphIntervalX, mCurrentReadingsGraphIntervalYLow, mCurrentReadingsGraphIntervalYHigh,
                                                     mTickCountX, mTickCountY);
        mController->setLevelsAndStatesAlarm(mLowLevelAlarm, mHighLevelAlarm, mLowLevelStateAlarm, mHighLevelStateAlarm);
        mICPSettings->setAllPressureParam(mCurrentReadingsGraphIntervalYLow, mCurrentReadingsGraphIntervalYHigh, mTickCountY,
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
                       mCurrentReadingsGraphIntervalYLow, mCurrentReadingsGraphIntervalYHigh, mTickCountX, mTickCountY]()
    {
        if (mController->setInetrvalsOnGraph(mCurrentReadingsGraphIntervalX, mCurrentReadingsGraphIntervalYLow, mCurrentReadingsGraphIntervalYHigh,
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
        //qDebug() << tempRegs[i];
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
    WFontScaling(ui->lowIntervalYLabel, scaleFactor);
    WFontScaling(ui->highIntervalYLabel, scaleFactor);
    WFontScaling(ui->intervalXLineEdit, scaleFactor);
    WFontScaling(ui->lowIntervalYLineEdit, scaleFactor);
    WFontScaling(ui->highIntervalYLineEdit, scaleFactor);
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
    WFontScaling(ui->aCoefficientLabel,             scaleFactor);
    WFontScaling(ui->aCoefficientLineEdit,          scaleFactor);
    WFontScaling(ui->bCoefficientLabel,             scaleFactor);
    WFontScaling(ui->bCoefficientLineEdit,          scaleFactor);

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

