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
    gridLayout = ui->gridLayout1;
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
    WFontScaling(ui->scrollArea, scaleFactor);
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
    resetLayout();
}

void ExportDataPage::resetLayout()
{
    clearLayout();
    QDir dir(mntDirectory);
    dir.setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    //QList <QLabel *> dirsContainer;
    QFileInfoList dirList = dir.entryInfoList();
    QFont fontLabel;
    fontLabel.setPixelSize(20);
    dirsItem *tempItem = new dirsItem;
    uint N = dirList.size();
    arrSize = N;
    qDebug() << "dirs" << N;
    dirsVector = new dirsItem[arrSize];
    for (uint i = 0; i < N; ++i)
    {
        QFileInfo fileInfo = dirList.at(i);
        QLabel* dirLabel = new QLabel();//ui->scrollArea//widget);
        dirLabel->setText(fileInfo.fileName());
        dirLabel->setFont(fontLabel);
        QCheckBox *checkBox = new QCheckBox();

        checkBox->setStyleSheet("QCheckBox::{backgraund-color:white;}");
#ifdef TEST_BUILD
        checkBox->setStyleSheet("QCheckBox::indicator {width: 20px; height: 20px;}");
#endif
        checkBox->setCheckable(true);
        checkBox->setFont(fontLabel);
        //tempItem = new dirsItem();
        tempItem->label = dirLabel;
        tempItem->checkBox = checkBox;
        //dirsVector.append(*tempItem);
        dirsVector[i] = *tempItem;
        gridLayout->addWidget(dirsVector[i].label, i, 0);
        gridLayout->addWidget(dirsVector[i].checkBox, i, 1);
        //delete dirLabel;
        //delete checkBox;
    }
    delete tempItem;
}

void ExportDataPage::clearLayout()
{
//    QLayoutItem *item;
//    while((item = gridLayout->takeAt(0)))
//    {
//        if (item->widget())
//        {
//            delete item->widget();
//            delete item;
//        }

//    }

    for (uint i=0; i<arrSize; i++)
    {
        delete dirsVector[i].label;
        delete dirsVector[i].checkBox;
    }

}
void ExportDataPage::selectAll()
{
    QCheckBox *currCheckBox{nullptr};
    uint N = gridLayout->rowCount();
    for (uint i=0; i<N; i++)
    {
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
        currCheckBox->setChecked(true);
    }
}

void ExportDataPage::clearSelection()
{
    QCheckBox *currCheckBox{nullptr};
    uint N = gridLayout->rowCount();
    for (uint i=0; i<N; i++)
    {
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
        currCheckBox->setChecked(false);
    }
}


void ExportDataPage::exportData()
{
    QCheckBox *currCheckBox{nullptr};
    uint N = gridLayout->rowCount();
    for (uint i=0; i<N; i++)
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
    //int N = ui->gridLayout1->rowCount();
    uint N = arrSize;
    //qDebug() << N;
    for (uint i=0; i<N; i++)
    {
        //QWidget *t0 = gridLayout->itemAtPosition(i, 0)->widget();
        //QWidget *t1 = gridLayout->itemAtPosition(i, 1)->widget();
        //currCheckBox = nullptr;
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
//        QWidget * widget1;
//        QWidget * widget2;
        //qDebug() << currCheckBox;
        if (currCheckBox->isChecked())
        {
            //qDebug() << i;
            currDirLabel = nullptr;
            currDirLabel = (QLabel*)gridLayout->itemAtPosition(i, 0)->widget();
            currDir = new QDir(mntDirectory+"/"+currDirLabel->text());

            //delete dirsVector[i]->label;
            //delete dirsVector[i]->checkBox;
            gridLayout->update();
            //delete widget1;
            //delete widget2;
            currDir->removeRecursively();
            currDir = nullptr;
        }
    }
    //qDebug() << gridLayout->rowCount() << "end";
    resetLayout();
}
