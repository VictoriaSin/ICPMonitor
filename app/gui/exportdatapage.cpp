#include "exportdatapage.h"
#include "ui_exportdatapage.h"
#include "ui_abstractdialogpage.h"

#include "gui/dialogWindows/messagedialog.h"
#include "gui_funcs.h"
#include "global_functions.h"

#include <QDebug>

extern QString mntDirectory;

ExportDataPage::ExportDataPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::ExportDataPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);
}

ExportDataPage::~ExportDataPage()
{
    delete ui;
}

void ExportDataPage::scaleFont(float scaleFactor)
{
    AbstractDialogPage::scaleFont(scaleFactor);
    WFontScaling(ui->selectAllButton, scaleFactor);
    WFontScaling(ui->clearAllSelectionsButton, scaleFactor);
    WFontScaling(ui->downloadButton, scaleFactor);
    WFontScaling(ui->deleteButton, scaleFactor);
}

void ExportDataPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Экспорт данных"));
    setBottomInfoLabel(tr(""));
}

void ExportDataPage::done(int exodus)
{
    if (exodus != QDialog::Accepted) {
        emit previousPage();
        return;
    }

    // Обновление всех параметров
    //updateParameters();
//    emit previousPage();
}

void ExportDataPage::showEvent(QShowEvent */*event*/)
{
    // ls /media/ICPMonitor/*/ -1 -d
    QString sessionDirs= executeAConsoleCommand("ls", QStringList() << "/media/ICPMonitor/*/" << "-1" << "-d");
    qDebug() << sessionDirs;
}
