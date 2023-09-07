#ifndef ALARMSETUPPAGE_H
#define ALARMSETUPPAGE_H

#include "abstractdialogpage.h"

namespace Ui {
class AlarmSetupPage;
}

class MessageDialog;

class AlarmSetupPage : public AbstractDialogPage
{
    Q_OBJECT

    const QString mAlarmEnabledSVG {":/icons/alarmEnabled.svg"};
    const QString mAlarmDisabledSVG {":/icons/alarmDisabled.svg"};

    /*! Допустимый диапазон нижнего уровня тревоги */
    const QPair<int, int> AdmissibleLowAlarmRange {qMakePair(-50, 150)};

    /*! Допустимый диапазон верхнего уровня тревоги */
    const QPair<int, int> AdmissibleHighAlarmRange {qMakePair(-50, 150)};

public:
    explicit AlarmSetupPage(QWidget *parent = nullptr);
    ~AlarmSetupPage();

private:
    /*! Настройка спинбокса тревоги */
    void setupAlarmSpinBoxes();

    /*! Настройки SVG виджетов */
    void setupSVGWidgets();

    /*! Настройка окна сообщений */
    void setupMessageBox();

    /*! Обновление виджетов с информацией о уровнях тревоги */
    void updateAlarmInfoWidgets();

    /*! Обновление виджетов текущих значений верхнего и нижнего уровней тревоги  */
    void updateAlarmLevelsOnWidgets();

    /*! Обновление виджетов текущих состояний верхнего и нижнего уровней тревоги */
    void updateAlarmStatesOnWidgets();

    /*! Обновление уровнений и состояний тревоги */
    void updateAlarmInfo();

    /*! Установка информации об ошибке при
        обновлении уровней и состояний тревоги
    */
    void openAlarmInfoErrorDialog(const QString &info);

private slots:
    /*! Изменение SVG для нижнего уровня тревоги */
    void changeLowLevelSVG(int state);

    /*! Изменение SVG для верхнего уровня тревоги */
    void changeHighLevelSVG(int state);

private:
    Ui::AlarmSetupPage *ui;

    /*! Диалоговое окно для уведомления пользователя */
    MessageDialog *mMessageDialog {nullptr};

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

public slots:
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

    // AbstractDialogPage interface
public slots:
    void done(int exodus) override;

};

#endif // ALARMSETUPPAGE_H
