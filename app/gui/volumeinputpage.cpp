#include "volumeinputpage.h"
#include "ui_volumeinputpage.h"
#include "ui_abstractdialogpage.h"

#include <QDialog>
#include <QDebug>


uint32_t windowWidth = 100;
float offsetAverage = 0.5;

uint16_t dVolume = 5;
float Po = 19.529; //30.0;
float Pk = 33.673; //30.02;


VolumeInputPage::VolumeInputPage(QWidget *parent, uint8_t type) :
    AbstractDialogPage(parent),
    ui(new Ui::VolumeInputPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);
    typeOfValues = type;
    if (typeOfValues == 0)
    {
        setupVolume();
    }
    else
    {
        setupParam();
    }
    // проскаалировать все
    retranslate();
}

void VolumeInputPage::setupVolume()
{
    dVolumeLabel = new QLabel("Укажите объем введенного болюса:", this);
    QFont fontLabel = dVolumeLabel->font();
    fontLabel.setPixelSize(20);
    dVolumeLabel->setFont(fontLabel);

    inputValueLineEdit = new QLineEdit(this);
    QFont fontLineEdit = inputValueLineEdit->font();
    fontLineEdit.setPixelSize(15);
    inputValueLineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    inputValueLineEdit->setFont(fontLineEdit);
    inputValueLineEdit->setText(QString::number(dVolume));

    point0Label = new QLabel("Укажите координату x точки P0:", this);
    point0Label->setFont(fontLabel);

    point0LineEdit = new QLineEdit(this);
    point0LineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    point0LineEdit->setFont(fontLineEdit);
    point0LineEdit->setText(QString::number(Po));

    point1Label = new QLabel("Укажите координату x точки Pk:", this);
    point1Label->setFont(fontLabel);

    point1LineEdit = new QLineEdit(this);
    point1LineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    point1LineEdit->setFont(fontLineEdit);
    point1LineEdit->setText(QString::number(Pk));

    ui->grid->addWidget(dVolumeLabel, 0, 0);
    ui->grid->addWidget(inputValueLineEdit, 0, 1);
    ui->grid->addWidget(point0Label, 1, 0);
    ui->grid->addWidget(point0LineEdit, 1, 1);
    ui->grid->addWidget(point1Label, 2, 0);
    ui->grid->addWidget(point1LineEdit, 2, 1);
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
    windowWidthLineEdit->setText(QString::number(windowWidth));

    offsetAverageLabel = new QLabel("Введите смещение относительно графика среднего:", this);
    offsetAverageLabel->setFont(fontLabel);

    offsetAverageLineEdit = new QLineEdit(this);
    offsetAverageLineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    offsetAverageLineEdit->setFont(fontLineEdit);
    offsetAverageLineEdit->setText(QString::number(offsetAverage));

    ui->grid->addWidget(windowWidthLabel, 0, 0);
    ui->grid->addWidget(windowWidthLineEdit, 0, 1);
    ui->grid->addWidget(offsetAverageLabel, 1, 0);
    ui->grid->addWidget(offsetAverageLineEdit, 1, 1);
}

#define DELITEM(_item) if(_item != nullptr)\
{\
    delete _item;\
    _item = nullptr;\
}

VolumeInputPage::~VolumeInputPage()
{
    DELITEM(dVolumeLabel);
    DELITEM(inputValueLineEdit);
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
        if (typeOfValues == 0)
        {
            inputValueLineEdit->setText(QString::number(startdVolume));
            point0LineEdit->setText(QString::number(startPo));
            point1LineEdit->setText(QString::number(startPk));
        }
        else // (typeOfValues == 1)
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
    if (typeOfValues == 0)
    {
        dVolume = inputValueLineEdit->text().toUInt();
        Po = point0LineEdit->text().toFloat();
        Pk = point1LineEdit->text().toFloat();
    }
    else
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
    if (typeOfValues == 0)
    {
        startdVolume = inputValueLineEdit->text().toUInt();
        startPo = point0LineEdit->text().toFloat();
        startPk = point1LineEdit->text().toFloat();
    }
    else //(typeOfValues == 1)
    {
        startValueWW = windowWidthLineEdit->text().toUInt();
        startValueOA = offsetAverageLineEdit->text().toFloat();
    }
}
