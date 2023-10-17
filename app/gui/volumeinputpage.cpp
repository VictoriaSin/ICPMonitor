#include "volumeinputpage.h"
#include "ui_volumeinputpage.h"
#include "ui_abstractdialogpage.h"

#include <QDialog>
#include <QDebug>


uint32_t windowWidth = 100;
float offsetAverage = 0.5;


VolumeInputPage::VolumeInputPage(QWidget *parent) :
    AbstractDialogPage(parent),
    ui(new Ui::VolumeInputPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);

    // проскаалировать все
    retranslate();
}

void VolumeInputPage::setupVolume()
{
    dVolume = new QLabel("Укажите объем введенного болюса:", this);
    QFont fontLabel = dVolume->font();
    fontLabel.setPixelSize(20);
    dVolume->setFont(fontLabel);

    inputValueLineEdit = new QLineEdit(this);
    QFont fontLineEdit = inputValueLineEdit->font();
    fontLineEdit.setPixelSize(15);
    inputValueLineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    inputValueLineEdit->setFont(fontLineEdit);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->leftLayout->addSpacerItem(spacer);
    ui->leftLayout->addWidget(dVolume);
    ui->leftLayout->addWidget(inputValueLineEdit);
    ui->leftLayout->addSpacerItem(spacer);
    delete spacer;
}

void VolumeInputPage::setupParam()
{
    windowWidthLabel = new QLabel("Введите ширину окна для расчета среднего:", this);
    QFont fontLabel = windowWidthLabel->font();
    fontLabel.setPixelSize(20);
    windowWidthLabel->setFont(fontLabel);

    windowWidthLineEdit = new QLineEdit(this);
    QFont fontLineEdit = windowWidthLineEdit->font();
    fontLineEdit.setPixelSize(15);
    windowWidthLineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    windowWidthLineEdit->setFont(fontLineEdit);
    windowWidthLineEdit->setText("100");

    offsetAverageLabel = new QLabel("Введите смещение относительно графика среднего:", this);
    offsetAverageLabel->setFont(fontLabel);

    offsetAverageLineEdit = new QLineEdit(this);
    offsetAverageLineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    offsetAverageLineEdit->setFont(fontLineEdit);
    offsetAverageLineEdit->setText("0.5");

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);

    ui->leftLayout->addSpacerItem(spacer);
    ui->leftLayout->addWidget(windowWidthLabel);
    ui->leftLayout->addWidget(windowWidthLineEdit);
    ui->leftLayout->addWidget(offsetAverageLabel);
    ui->leftLayout->addWidget(offsetAverageLineEdit);
    ui->leftLayout->addSpacerItem(spacer);
    delete spacer;
}

#define DELITEM(_item) if(_item != nullptr)\
{\
    delete _item;\
    _item = nullptr;\
}

VolumeInputPage::~VolumeInputPage()
{
    //DELITEM(dVolume);
    //DELITEM(inputValueLineEdit);
    DELITEM(windowWidthLabel);
    DELITEM(windowWidthLineEdit);
    DELITEM(offsetAverageLabel);
    DELITEM(offsetAverageLineEdit);
    delete ui;
}

bool VolumeInputPage::checkValue(QString inputString)
{
    QRegExp mRegExp("(0|[1-9][\\d]*)(\\.[\\d]+)?");
    return mRegExp.exactMatch(inputString);
}

void VolumeInputPage::done(int exodus)
{
    if (exodus != QDialog::Accepted) {        
        emit previousPage();
        if (this->findChildren<QLineEdit*>().size() != 1)
        {
            windowWidthLineEdit->setText(QString::number(startValueWW));
            offsetAverageLineEdit->setText(QString::number(startValueOA));
        }
        return;
    }
    QList<QLineEdit*> list = this->findChildren<QLineEdit*>();
    //qDebug() << list;
    for (uint8_t i=0; i<list.size(); i++)
    {
        if (!checkValue(list[i]->text()))
        {
            return;
        }
    }
    if (list.size() > 1)
    {
        windowWidth = windowWidthLineEdit->text().toUInt();
        offsetAverage = offsetAverageLineEdit->text().toFloat();
        emit intervalParamUpdated();
    }
    emit previousPage();

}

void VolumeInputPage::retranslate()
{
    AbstractDialogPage::retranslate();
    ui->retranslateUi(this);

    setUpperNamePageLabel(tr("Ввод параметров"));
    setBottomInfoLabel(tr(""));
}

void VolumeInputPage::showEvent(QShowEvent */*event*/)
{
    if (this->findChildren<QLineEdit*>().size() != 1)
    {
    startValueWW = windowWidthLineEdit->text().toUInt();
    startValueOA = offsetAverageLineEdit->text().toFloat();
    }
}
