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
#include "global_define.h"

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
    connect(ui->deleteButton, &QPushButton::clicked, [this](){
        mDeleteDirsDialog->setTextMessage("Вы точно хотите удалить\nвыбранные директории?");
        mDeleteDirsDialog->open(); });

    enableAcceptButton(false);
    setIconRejectButton(QIcon(":/icons/goBack.svg"), QIcon(":/icons/goBack_pressed.svg"));

    mDeleteDirsDialog = new MessageDialog(this);

    // Настраиваем кнопку
    mDeleteDirsDialog->mOkeyButton->hide();
    AcceptButton = new QPushButton(this);
    AcceptButton->setText("Accept");
    mDeleteDirsDialog->addButton(AcceptButton);

    CancelButton = new QPushButton(this);
    CancelButton->setText("Cancel");
    mDeleteDirsDialog->addButton(CancelButton);
    connect(AcceptButton, &QPushButton::clicked, this, &ExportDataPage::deleteDirs);
    connect(CancelButton, &QPushButton::clicked, mDeleteDirsDialog, &QDialog::reject);

}
#define CORRECTDELETE(_item)\
if (_item != nullptr)\
{\
    delete _item;\
    _item = nullptr;\
}

ExportDataPage::~ExportDataPage()
{
    CORRECTDELETE(dirLabel);
    CORRECTDELETE(checkBox);
    CORRECTDELETE(gridLayout);
    CORRECTDELETE(AcceptButton);
    CORRECTDELETE(CancelButton);
    if (dirsVector != nullptr)
    {
        delete[] dirsVector;
        dirsVector = nullptr;
    }
    DESTROY_CLASS(mDeleteDirsDialog);
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
    mDeleteDirsDialog->scaleFont(scaleFactor);
}

void ExportDataPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Управление данными"));
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
    if (dirsVector != nullptr)
    {
        clearLayout();
    }
    QDir dir(mntDirectory);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    //QList <QLabel *> dirsContainer;
    QFileInfoList dirList = dir.entryInfoList();
    QFont fontLabel;
    fontLabel.setPixelSize(20);
    dirsItem *tempItem = new dirsItem;
    uint N = dirList.size();
    arrSize = N;
    qDebug() << "dirs" << N;
    dirsVector = new dirsItem[arrSize];
    int notCorrectDirCount = 0;
    QRegExp mRegExp("[0-9]{4}(_[0-9]{2}){2}@[0-9]{2}(_[0-9]{2}){2}");
    for (uint i = 0; i < N; ++i)
    {
        QFileInfo fileInfo = dirList.at(i);
        if (mRegExp.exactMatch(fileInfo.fileName()))//(fileInfo.fileName().contains(mRegExp))
        {
            dirLabel = new QLabel();//ui->scrollArea//widget);
            dirLabel->setText(fileInfo.fileName());
            dirLabel->setFont(fontLabel);
            checkBox = new QCheckBox();

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
            gridLayout->addWidget(dirsVector[i].label, i-notCorrectDirCount, 0);
            gridLayout->addWidget(dirsVector[i].checkBox, i-notCorrectDirCount, 1);
        }
        else
        {
            notCorrectDirCount++;
        }

    }
    arrSize = N-notCorrectDirCount;
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
    mDeleteDirsDialog->accept();
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
            delete currDir;
            currDir = nullptr;
        }
    }
    //qDebug() << gridLayout->rowCount() << "end";
    resetLayout();
}

void ExportDataPage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);
    retranslate();
}
