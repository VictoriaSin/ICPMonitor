#include "movedatadialog.h"
#include "ui_movedatadialog.h"
#include "ui_abstractdialogwindow.h"
#include "gui/gui_funcs.h"

MoveDataDialog::MoveDataDialog(QWidget *parent) :
    AbstractDialogWindow(parent),
    ui(new Ui::MoveDataDialog)
{
    ui->setupUi(AbstractDialogWindow::ui->widget);
}

MoveDataDialog::~MoveDataDialog()
{
    delete ui;
}

void MoveDataDialog::setRange(int minimum, int maximum)
{
    ui->progressBar->setRange(minimum, maximum);
}

void MoveDataDialog::setMovingFileText(const QString &text)
{
    ui->infoLabel->setText(text);
}

void MoveDataDialog::setValue(int value)
{
    ui->progressBar->setValue(value);
}

void MoveDataDialog::setProgressText(const QString &text)
{
    ui->progressInfo->setText(text);
}

void MoveDataDialog::scaleFont(float scaleFactor)
{
    WFontScaling(ui->infoLabel, scaleFactor);
    WFontScaling(ui->progressInfo, scaleFactor);
    WFontScaling(ui->warningLabel, scaleFactor);
    WFontScaling(ui->progressBar, scaleFactor);
}

void MoveDataDialog::retranslate()
{
    ui->retranslateUi(this);
}

void MoveDataDialog::showEvent(QShowEvent *event)
{
    AbstractDialogWindow::showEvent(event);
}
