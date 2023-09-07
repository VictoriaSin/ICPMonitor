#ifndef ABSTRACTDIALOGWINDOW_H
#define ABSTRACTDIALOGWINDOW_H

#include <QDialog>

namespace Ui {
class AbstractDialogWindow;
}

class AbstractDialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AbstractDialogWindow(QWidget *parent = nullptr);
    ~AbstractDialogWindow();

    virtual void scaleFont(float scaleFactor) = 0;
    virtual void retranslate() = 0;

    /*! Устанавливает виджет по центру */
    void setCenter();

protected:
    Ui::AbstractDialogWindow *ui;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // ABSTRACTDIALOGWINDOW_H
