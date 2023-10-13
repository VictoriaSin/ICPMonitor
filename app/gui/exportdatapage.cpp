#include "exportdatapage.h"
#include "ui_exportdatapage.h"
#include "ui_abstractdialogpage.h"

#include "gui/dialogWindows/messagedialog.h"
#include "gui_funcs.h"
#include "global_functions.h"

#include <QDebug>
#include <QDir>
#include <QCheckBox>
#include <QPushButton>

extern QString mntDirectory;

ExportDataPage::ExportDataPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::ExportDataPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);
    scrollArea1 = new QScrollArea(this);
    ui->verticalLayout_2->addWidget(scrollArea1);
    scrollArea1->setWidgetResizable(true);
    widget = new QWidget();
    gridLayout = new QGridLayout(widget);
    scrollArea1->setWidget(widget);
    connect(ui->selectAllButton, &QPushButton::clicked, this, &ExportDataPage::selectAll);
    connect(ui->clearAllSelectionsButton, &QPushButton::clicked, this, &ExportDataPage::clearSelection);
    connect(ui->downloadButton, &QPushButton::clicked, this, &ExportDataPage::exportData);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ExportDataPage::deleteDirs);
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
    QDir dir(mntDirectory);
    dir.setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    //QList <QLabel *> dirsContainer;
    QFileInfoList dirList = dir.entryInfoList();
    QFont fontLabel;
    fontLabel.setPixelSize(20);
//    if (gridLayout->rowCount()!=1)
//    {
//    //for (int i = 0; i < gridLayout->rowCount(); i++)

////    {
////        gridLayout->removeWidget(gridLayout->itemAt(i)->widget());
////    }

//    }
//    if ( gridLayout->layout() != NULL )
//    {
//        QLayoutItem* item;
//        while ( ( item = gridLayout->layout()->takeAt( 0 ) ) != NULL )
//        {
//            //item = gridLayout->layout()->itemAt(i);
//            delete item->widget();
//            delete item;
//        }
//    }
    clearLayout(gridLayout);
    for (int i = 0; i < dirList.size(); ++i)
    {
        QFileInfo fileInfo = dirList.at(i);
        QLabel* dirLabel = new QLabel(widget);
        dirLabel->setText(fileInfo.fileName());
        dirLabel->setFont(fontLabel);
        QCheckBox *checkBox = new QCheckBox();
        checkBox->setStyleSheet("QCheckBox::{backgraund-color:white;}"); //"QCheckBox::indicator {color:black;}");
        checkBox->setCheckable(true);
        checkBox->setFont(fontLabel);
        gridLayout->addWidget(dirLabel, i, 0);
        gridLayout->addWidget(checkBox, i, 1);
    }

}

void ExportDataPage::clearLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(0)))
    {
        if (item->widget())
        {
            delete item->widget();
        }
    }
}
void ExportDataPage::selectAll()
{
    QCheckBox *currCheckBox{nullptr};
    for (int i=0; i<gridLayout->rowCount(); i++)
    {
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
        currCheckBox->setChecked(true);
    }
}

void ExportDataPage::clearSelection()
{
    QCheckBox *currCheckBox{nullptr};
    for (int i=0; i<gridLayout->rowCount(); i++)
    {
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
        currCheckBox->setChecked(false);
    }
}


void ExportDataPage::exportData()
{
    QCheckBox *currCheckBox{nullptr};
    for (int i=0; i<gridLayout->rowCount(); i++)
    {
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
        if (currCheckBox->isChecked())
        {
            //
        }
    }
}

void ExportDataPage::deleteDirs()
{
    QCheckBox *currCheckBox {nullptr};
    QLabel *currDirLabel {nullptr};
    QDir *currDir {nullptr};
    for (int i=0; i<gridLayout->rowCount(); i++)
    {
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
        QLayoutItem* item;
        if (currCheckBox->isChecked())
        {
            currDirLabel = (QLabel*)gridLayout->itemAtPosition(i, 0)->widget();
            currDir = new QDir(mntDirectory+"/"+currDirLabel->text());

            currDir->removeRecursively();
            //qDebug() << executeAConsoleCommand("rm", QStringList() << "-r" << mntDirectory+"/"+currDir->text());
        }
    }
}
