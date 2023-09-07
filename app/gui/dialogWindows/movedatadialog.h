#ifndef MOVEDATADIALOG_H
#define MOVEDATADIALOG_H

#include "abstractdialogwindow.h"

namespace Ui {
class MoveDataDialog;
}

class MoveDataDialog : public AbstractDialogWindow
{
    Q_OBJECT

public:
    explicit MoveDataDialog(QWidget *parent = nullptr);
    ~MoveDataDialog();

    /*! Установка значений шкалы прогресса */
    void setRange(int minimum, int maximum);

    /*! Установка информационного текста */
    void setMovingFileText(const QString &text);

    /*! Установка текущего значения */
    void setValue(int value);

    /*! Установка текста прогресса */
    void setProgressText(const QString &text);

private:
    Ui::MoveDataDialog *ui;

    // AbstractDialogWindow interface
public:
    void scaleFont(float scaleFactor) override;
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

};

#endif // MOVEDATADIALOG_H
