#ifndef GENERALSETTINGSPAGE_H
#define GENERALSETTINGSPAGE_H

#include "abstractdialogpage.h"
#include "ui_generalsettingspage.h"
#include "global_define.h"
#include "plots/recordedplot.h"



namespace Ui {
class GeneralSettingsPage;
}

class MessageDialog;

extern float mCurrentMaxYRange;

class GeneralSettingsPage : public AbstractDialogPage
{
    Q_OBJECT

    QList<QLineEdit*> mLineEditList;
public:
    explicit GeneralSettingsPage(QWidget *parent = nullptr);
    ~GeneralSettingsPage();

    void scaleFont(float scaleFactor) override;
    void installController(MonitorController *controller) override;

    bool eventFilter(QObject *o, QEvent *e) override;
private:
    Ui::GeneralSettingsPage *ui;

    /*! Диалоговое окно для уведомления пользователя */
    MessageDialog *mMessageDialog {nullptr};


    /*! Обновление виджетов тревоги */
    void updateAlarmSettingsOnWidgets();

    /*! Обновление виджетов графика */
    void updateGraphSettingsOnWidgets();

    /*! Обновление группы основных настроек */
    void updateGeneralSettingsOnWidgets();

    void setupAlarm();

    void updateParameters();

    /*! Проверка на корректность ввода*/
    bool checkInputData(QString inputParametr);

    /*! Настройка окна сообщений */
    void setupMessageBox();

    /*! Установка информации об ошибке при
        обновлении настроек */
    void openSettingsInfoErrorDialog(const QString &info);

    QLineEdit* currLineEdit;

private slots:
    void changeAlarmStatus();
    void editLine(QString);

public slots:
    void retranslate() override;
    void done(int exodus) override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
    bool checkInputRegs(QString inputReg);
    bool checkInputCoeff(QString inputCoeff);
};

#endif // GENERALSETTINGSPAGE_H
