#ifndef TEXTEDITDIALOG_H
#define TEXTEDITDIALOG_H

#include "abstractdialogwindow.h"
#include "QToolButton"

namespace Ui {
class TextEditDialog;
}

class TextEditDialog : public AbstractDialogWindow
{
    Q_OBJECT

public:
    explicit TextEditDialog(QWidget *parent = nullptr);
    ~TextEditDialog();

    /*! Установка информационного текста */
    void setInfoLabelText(const QString &text);

    /*! Установка текста в поле ввода */
    void setText(const QString &text);

    /*! Текст в поле ввода */
    QString getText() const;

private:
    Ui::TextEditDialog *ui;

    /*! Кнопка подтверждения */
    QToolButton *mAcceptButton {nullptr};

    /*! Кнопка отмены */
    QToolButton *mRejectButton {nullptr};

    // AbstractDialogWindow interface
public:
    void scaleFont(float scaleFactor) override;
    void retranslate() override;
};

#endif // TEXTEDITDIALOG_H
