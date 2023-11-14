#include "alarmsetuppage.h"
#include "ui_alarmsetuppage.h"
#include "ui_abstractdialogpage.h"
#include "controller/monitorcontroller.h"
#include "controller/settings.h"
#include "gui/dialogWindows/messagedialog.h"
#include "gui_funcs.h"

#include <QSpinBox>
#include <QTimer>
#include <QDebug>

AlarmSetupPage::AlarmSetupPage(QWidget *parent) :
  AbstractDialogPage(parent),
  ui(new Ui::AlarmSetupPage)
{
  ui->setupUi(AbstractDialogPage::ui->settingsPage);
  setupSVGWidgets();

  // Настройка спинбоксов
  setupAlarmSpinBoxes();

  // Настройка окна сообщений
  setupMessageBox();
  qDebug("\033[34m>>AlarmSetupPage::AlarmSetupPage\033[0m");
}

AlarmSetupPage::~AlarmSetupPage()
{
  qDebug("\033[34m<<AlarmSetupPage::AlarmSetupPage\033[0m");
  delete ui;
}

void AlarmSetupPage::setupAlarmSpinBoxes()
{
  if (ui->inputHighAlarmDigits->currentSpinBox() && ui->inputLowAlarmDigits->currentSpinBox()) { return; }

  // Настройка спинбокса нижнего уровня тревоги
  QSpinBox *lowSpinBox = new QSpinBox(ui->inputLowAlarmDigits);
  lowSpinBox->setMinimumWidth(100);
  ui->inputLowAlarmDigits->setSpinBox(lowSpinBox);
  ui->inputLowAlarmDigits->setRange(AdmissibleLowAlarmRange.first, AdmissibleLowAlarmRange.second);

  // Настройка спинбокса верхнего уровня тревоги
  QSpinBox *highSpinBox = new QSpinBox(ui->inputHighAlarmDigits);
  highSpinBox->setMinimumWidth(100);
  ui->inputHighAlarmDigits->setSpinBox(highSpinBox);
  ui->inputHighAlarmDigits->setRange(AdmissibleHighAlarmRange.first, AdmissibleHighAlarmRange.second);
}

void AlarmSetupPage::setupSVGWidgets()
{
  // Установка размера SVG картинок
  ui->highAlarmSvgWidget->setMaximumSize(75, 75);
  ui->lowAlarmSvgWidget->setMaximumSize(75, 75);

  // При смене состояния чекбоса меняем SVG (вкл/выкл тревога)
  connect(ui->lowAlarmCheckbox, &QCheckBox::stateChanged, this, &AlarmSetupPage::changeLowLevelSVG);
  connect(ui->highAlarmCheckbox, &QCheckBox::stateChanged, this, &AlarmSetupPage::changeHighLevelSVG);
}

void AlarmSetupPage::setupMessageBox()
{
  if (mMessageDialog) {
    return;
  }
  mMessageDialog = new MessageDialog(this);
}

void AlarmSetupPage::updateAlarmInfoWidgets()
{
  updateAlarmLevelsOnWidgets();
  updateAlarmStatesOnWidgets();
}

void AlarmSetupPage::updateAlarmLevelsOnWidgets()
{
  if (!mController) {
    return;
  }

  const Settings * const settings = mController->settings();
  if (!settings) {
    return;
  }

  ui->inputLowAlarmDigits->setValue(settings->getLowLevelAlarm());
  ui->inputHighAlarmDigits->setValue(settings->getHighLevelAlarm());
}

void AlarmSetupPage::updateAlarmStatesOnWidgets()
{
  if (!mController) {
    return;
  }

  const Settings * const settings = mController->settings();
  if (!settings) {
    return;
  }

  const bool currentLowState = settings->getLowLevelStateAlarm();
  const bool currentHighState = settings->getHighLevelStateAlarm();

  ui->lowAlarmCheckbox->setChecked(currentLowState);
  ui->highAlarmCheckbox->setChecked(currentHighState);

  changeLowLevelSVG(currentLowState);
  changeHighLevelSVG(currentHighState);
}


void AlarmSetupPage::updateAlarmInfo()
{
  if (!mController) {
    return;
  }

  // Состояния тревоги (Вкл/Выкл)
  bool lowState = ui->lowAlarmCheckbox->isChecked();
  bool highState = ui->highAlarmCheckbox->isChecked();

  // Уровни тревоги
  bool LL {false};
  bool HL {false};
  const auto lowLevel = ui->inputLowAlarmDigits->value(&LL);
  const auto highLevel = ui->inputHighAlarmDigits->value(&HL);
  if (!LL || !HL) {
    return;
  }

  // Устанавливаем
  QTimer::singleShot(0, mController, [this, lowLevel, highLevel, lowState, highState](){
    if (mController->setLevelsAndStatesAlarm(lowLevel, highLevel, lowState, highState)) {
      emit previousPage();
    } else {
      openAlarmInfoErrorDialog(tr("Верхний уровень должен\nбыть больше, чем нижний!"));
    }
  });
}

void AlarmSetupPage::openAlarmInfoErrorDialog(const QString &info)
{
  mMessageDialog->setTextMessage(info);
  mMessageDialog->open();
}

void AlarmSetupPage::changeLowLevelSVG(int state)
{
  ui->lowAlarmSvgWidget->load(state ? mAlarmEnabledSVG : mAlarmDisabledSVG);
}

void AlarmSetupPage::changeHighLevelSVG(int state)
{
  ui->highAlarmSvgWidget->load(state ? mAlarmEnabledSVG : mAlarmDisabledSVG);
}

void AlarmSetupPage::installController(MonitorController *controller)
{
  AbstractDialogPage::installController(controller);
  retranslate();
}

void AlarmSetupPage::scaleFont(float scaleFactor)
{
  AbstractDialogPage::scaleFont(scaleFactor);
  mMessageDialog->scaleFont(scaleFactor);
  WFontScaling(ui->measurementLabel, scaleFactor);
  WFontScaling(ui->measurementLabel2, scaleFactor);
  WFontScaling(ui->highAlarmLabel, scaleFactor);
  WFontScaling(ui->lowAlarmLabel, scaleFactor);
  WFontScaling(ui->inputLowAlarmDigits, scaleFactor);
  WFontScaling(ui->inputHighAlarmDigits, scaleFactor);
  WFontScaling(ui->keyboardWidget, scaleFactor);
  WFontScaling(ui->highAlarmCheckbox, scaleFactor);
}

void AlarmSetupPage::retranslate()
{
  AbstractDialogPage::retranslate();
  ui->retranslateUi(this);

  setUpperNamePageLabel(tr("Настройка тревоги по ВЧД"));
  setBottomInfoLabel(tr("Верхний уровень срабатывания тревоги"
                        " должен\n быть больше, чем нижний уровень!"));

  mMessageDialog->retranslate();
}

void AlarmSetupPage::done(int exodus)
{
  if (exodus != QDialog::Accepted) {
    emit previousPage();
    return;
  }

  // Обновление уровней и состояний тревоги
  updateAlarmInfo();
}

void AlarmSetupPage::showEvent(QShowEvent */*event*/)
{
  updateAlarmInfoWidgets();
}
