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
        mDeleteDirsDialog->setTextMessage(tr("Вы уверены, что хотите удалить\nвыбранные директории?"));
        mDeleteDirsDialog->open(); });

    enableAcceptButton(false);
    setIconRejectButton(QIcon(":/icons/goBack.svg"), QIcon(":/icons/goBack_pressed.svg"));

    mDeleteDirsDialog = new MessageDialog(this);

    // Настраиваем кнопку
    mDeleteDirsDialog->mOkeyButton->hide();
    AcceptButton = new QPushButton(this);
    AcceptButton->setText(tr("Да"));
    mDeleteDirsDialog->addButton(AcceptButton);

    CancelButton = new QPushButton(this);
    CancelButton->setText(tr("Нет"));
    mDeleteDirsDialog->addButton(CancelButton);
    connect(AcceptButton, &QPushButton::clicked, this, &ExportDataPage::deleteDirs);
    connect(CancelButton, &QPushButton::clicked, mDeleteDirsDialog, &QDialog::reject);

    flashTimer.setInterval(2000);
    connect(&flashTimer, &QTimer::timeout, this, &ExportDataPage::findFlash);
    ui->downloadButton->setEnabled(false);
    qDebug("\033[34m>>ExportDataPage::ExportDataPage\033[0m");
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
    qDebug("\033[34m<<ExportDataPage::~ExportDataPage\033[0m");
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
    AcceptButton->setText(tr("Да"));
    CancelButton->setText(tr("Нет"));
    mDeleteDirsDialog->retranslate();
}

void ExportDataPage::done(int exodus)
{
    if (exodus != QDialog::Accepted)
    {
        flashTimer.stop();
        mFlashStatusLabel->hide();
        //if (executeAConsoleCommand("umount", QStringList() << "/dev/"+part_ForSave) != "")
        {
                qDebug() << executeAConsoleCommand("umount", QStringList() << "/dev/"+part_ForSave);
        }
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
    flashTimer.start();
    mFlashStatusLabel->show();
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
    flashTimer.stop();
    mFlashStatusLabel->setText("Exporting... Don't remove flash device");
    if (isFlashAvailable)
    {        
        QString mntSaveDataDir("/media/ExportData");
        QDir mSaveOnFlashDir(mntSaveDataDir);
        if (!mSaveOnFlashDir.exists())
        {
            QString outputString = executeAConsoleCommand("mkdir", QStringList() << "-m" << "777" << mntSaveDataDir);
            if (outputString != "")
            {
                qDebug() << "Result" << outputString;
                return;
            }
        }
        QString result = executeAConsoleCommand("mount", QStringList() << "/dev/"+part_ForSave << mntSaveDataDir);
        if (result != "")
        {
          qDebug() << "Result" << result;
          return;
        }
        QCheckBox *currCheckBox{nullptr};
        uint N = gridLayout->rowCount();
        uint inDirSize = 0;
        uint outDirSize = 0;


        for (uint i=0; i<N; i++)
        {
            currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
            QString ttt = ((QLabel*)gridLayout->itemAtPosition(i, 0)->widget())->text();
            if (currCheckBox->isChecked())            {

                qDebug() << executeAConsoleCommand("cp", QStringList() << "-R" << mntDirectory+"/"+ttt << mntSaveDataDir);
                inDirSize = executeAConsoleCommand("du", QStringList() << "-s" << mntDirectory+"/"+ttt).split("\t")[0].toUInt();
                qDebug() << inDirSize;
                outDirSize = executeAConsoleCommand("du", QStringList() << "-s" << mntSaveDataDir+"/"+ttt).split("\t")[0].toUInt();
                qDebug() << outDirSize;
                if (inDirSize != outDirSize)
                {
                    currCheckBox->setChecked(false);
                }
            }
        }        
        deleteDirs();
    }
    flashTimer.start();
}

void ExportDataPage::findFlash()
{
    QStringList mDevicesByUUIDList;
    QStringList cmdAllDevList = executeAConsoleCommand("ls", QStringList() << "-l" << "/dev/disk/by-uuid").split("\n");
    uint8_t tempIndex = 0;
    QString rasdel;
    QString UUID_ForSave;

    for (uint8_t i = 1; i< cmdAllDevList.count()-1; i++)
    {
      tempIndex = cmdAllDevList[i].split(" ").indexOf("->");
      if (!(cmdAllDevList[i].split(" ")[tempIndex-1].contains("-"))) //ntfs
      {
          // UUID + " " + rasdel
        mDevicesByUUIDList.append(cmdAllDevList[i].split(" ")[tempIndex-1] + " " + cmdAllDevList[i].split(" ")[tempIndex+1].split("/")[2]);
      }
    }
    if ((mDevicesByUUIDList.isEmpty()) || (mDevicesByUUIDList.size() == 1))
    {
        qDebug() << "NO FLASH";
        mFlashStatusLabel->setText("No flash device");
        isFlashAvailable = false;
        ui->downloadButton->setEnabled(false);
    }
    else
    {
        for (uint8_t i = 0; i< mDevicesByUUIDList.count(); i++)
        {
            if (mDevicesByUUIDList[i].split(" ")[0] != mICPSettings->getSoftwareStorageUUID())
            {
                UUID_ForSave = mDevicesByUUIDList[i].split(" ")[0];
                part_ForSave = mDevicesByUUIDList[i].split(" ")[1];
                qDebug() << "UUID" << UUID_ForSave << "part" << part_ForSave;
                mFlashStatusLabel->setText("Flash device: " + UUID_ForSave);
                isFlashAvailable = true;
                ui->downloadButton->setEnabled(true);
            }
        }
    }
}

void ExportDataPage::deleteDirs()
{
    mDeleteDirsDialog->accept();
    QCheckBox *currCheckBox {nullptr};
    QLabel *currDirLabel {nullptr};
    QDir *currDir {nullptr};
    uint N = arrSize;
    for (uint i=0; i<N; i++)
    {
        currCheckBox = (QCheckBox*)gridLayout->itemAtPosition(i, 1)->widget();
        if (currCheckBox->isChecked())
        {
            currDirLabel = nullptr;
            currDirLabel = (QLabel*)gridLayout->itemAtPosition(i, 0)->widget();
            currDir = new QDir(mntDirectory+"/"+currDirLabel->text());
            gridLayout->update();
            currDir->removeRecursively();
            delete currDir;
            currDir = nullptr;
        }
    }
    resetLayout();
}

void ExportDataPage::installController(MonitorController *controller)
{
    AbstractDialogPage::installController(controller);    
}
