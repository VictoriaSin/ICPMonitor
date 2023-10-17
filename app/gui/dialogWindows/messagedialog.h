#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include "abstractdialogwindow.h"

namespace Ui {
class MessageDialog;
}

class MessageDialog : public AbstractDialogWindow
{
    Q_OBJECT

public:
    explicit MessageDialog(QWidget *parent = nullptr);
    ~MessageDialog();

    /*! Установка сообщения в диалоговое окно */
    void setTextMessage(const QString &info);

private:
    Ui::MessageDialog *ui;


    QFont mButtonFont;

    // AbstractDialogWindow interface
public:
    void scaleFont(float scaleFactor) override;
    void retranslate() override;

    QPushButton *mOkeyButton {nullptr};

    // QWidget interface
    void addButton(QPushButton *btn);
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // MESSAGEDIALOG_H
