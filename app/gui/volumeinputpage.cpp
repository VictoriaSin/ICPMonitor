#include "volumeinputpage.h"
#include "ui_volumeinputpage.h"
#include "ui_abstractdialogpage.h"

#include <QDialog>

VolumeInputPage::VolumeInputPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::VolumeInputPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);
    retranslate();
}

VolumeInputPage::~VolumeInputPage()
{
    delete ui;
}

bool VolumeInputPage::checkValue()
{
    QRegExp mRegExp("(0|[1-9][\\d]*)(\\.[\\d]+)?");
    return mRegExp.exactMatch(ui->inputValueLineEdit->text());
}

void VolumeInputPage::done(int exodus)
{
    if (exodus != QDialog::Accepted) {
        emit previousPage();
        return;
    }
    if (checkValue())
    {
        emit previousPage();
    }
}

void VolumeInputPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Ввод параметров"));
    setBottomInfoLabel(tr(""));
}
