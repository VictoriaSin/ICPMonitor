#include "messagedialog.h"
#include "ui_messagedialog.h"
#include "ui_abstractdialogwindow.h"
#include "gui/gui_funcs.h"

#include <QPushButton>
#include <QDebug>

MessageDialog::MessageDialog(QWidget *parent) :
    AbstractDialogWindow(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(AbstractDialogWindow::ui->widget);

    // Настройка шрифта для кнопки
    mButtonFont.setFamily("Sans Serif");
    mButtonFont.setPointSize(30);

    // Настраиваем кнопку
    mOkeyButton = new QPushButton(this);
    mOkeyButton->setFont(mButtonFont);

    // Добавляем кнопку на форму
    AbstractDialogWindow::ui->buttonLayout->addWidget(mOkeyButton);

    // По нажатию на кнопку диалог закрывается с результатом Accept
    connect(mOkeyButton, &QPushButton::clicked, this, &QDialog::accept);
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

void MessageDialog::setTextMessage(const QString &info)
{
    ui->infoLabel->setText(info);
}

void MessageDialog::scaleFont(float scaleFactor)
{
    WFontScaling(ui->infoLabel, scaleFactor);
    FontScaling(mButtonFont, scaleFactor);
    mOkeyButton->setFont(mButtonFont);
}

void MessageDialog::retranslate()
{
    // Название окна
    setWindowTitle(tr("Информация"));
    mOkeyButton->setText(tr("Ок"));
}

void MessageDialog::showEvent(QShowEvent *event)
{
    AbstractDialogWindow::showEvent(event);
}
