#ifndef PATIENTINFODIALOG_H
#define PATIENTINFODIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <../app/gui/mykboard.h>
namespace Ui {
class PatientInfoDialog;
}

class PatientInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PatientInfoDialog(QWidget *parent = nullptr);
    ~PatientInfoDialog();

    void setLabel(QString newLabel);
    void setPatientDialog();
    void setInfoDialog();
private slots:
    void on_openKeyboardButton_clicked();

private:
    Ui::PatientInfoDialog *ui;
    void setCenter();
    bool kboardBtnStatus;
    QLineEdit *currLineEdit;

    enum inputWidget {line, text};
    bool currInput;

    bool checkInputData(QString inputParametr);
    QString dirName;
public slots:
    void printChar(QString);
protected:
    void showEvent(QShowEvent *event);


    // QObject interface
    void acceptPatientInfo();
public:
    bool eventFilter(QObject *watched, QEvent *event);
    QString getDirName();
};

#endif // PATIENTINFODIALOG_H
