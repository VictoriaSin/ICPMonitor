#include "texteditdialog.h"
#include "ui_texteditdialog.h"
#include "ui_abstractdialogwindow.h"
#include "gui/gui_funcs.h"

TextEditDialog::TextEditDialog(QWidget *parent) :
    AbstractDialogWindow(parent),
    ui(new Ui::TextEditDialog)
{
    ui->setupUi(AbstractDialogWindow::ui->widget);

    // Настраиваем кнопку подтверждения
    mAcceptButton = new QToolButton(this);
    mAcceptButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    mAcceptButton->setIconSize(QSize(225, 75));
    mAcceptButton->setStyleSheet("QToolButton {"
                                 "background-color: rgba(255, 255, 255, 0);"
                                 "border: none;"
                                 "color: white;"
                                 "font: bold;"
                                 "image: url(\":/icons/acceptWide.svg\");"
                                 "}"

                                 "QToolButton:pressed {"
                                 "image: url(\":/icons/acceptWide_pressed.svg\");"
                                 "}");

    // Добавляем кнопку на форму
    AbstractDialogWindow::ui->buttonLayout->addWidget(mAcceptButton);

    // По нажатию на кнопку согласия диалог закрывается с результатом Accept
    connect(mAcceptButton, &QToolButton::clicked, this, &QDialog::accept);

    // Настраиваем кнопку отмены
    mRejectButton = new QToolButton(this);
    mRejectButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    mRejectButton->setIconSize(QSize(225, 75));
    mRejectButton->setStyleSheet("QToolButton {"
                                 "background-color: rgba(255, 255, 255, 0);"
                                 "border: none;"
                                 "color: white;"
                                 "font: bold;"
                                 "image: url(\":/icons/rejectWide.svg\");"
                                 "}"

                                 "QToolButton:pressed {"
                                 "image: url(\":/icons/rejectWide_pressed.svg\");"
                                 "}");

    // Добавляем кнопку на форму
    AbstractDialogWindow::ui->buttonLayout->addWidget(mRejectButton);

    // По нажатию на кнопку отмены диалог закрывается с результатом Reject
    connect(mRejectButton, &QToolButton::clicked, this, &QDialog::reject);
}

TextEditDialog::~TextEditDialog()
{
    delete ui;
}

void TextEditDialog::setInfoLabelText(const QString &text)
{
    ui->infoLabel->setText(text);
}

void TextEditDialog::setText(const QString &text)
{
    ui->textEdit->setText(text);

    // Передвигаем курсор в конец
    QTextCursor newCursor = ui->textEdit->textCursor();
    newCursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(newCursor);
}

QString TextEditDialog::getText() const
{
    return ui->textEdit->toPlainText();
}

void TextEditDialog::scaleFont(float scaleFactor)
{
    WFontScaling(ui->textEdit, scaleFactor);
    WFontScaling(ui->infoLabel, scaleFactor);
}

void TextEditDialog::retranslate()
{

}
