#include "volumeinputpage.h"
#include "ui_volumeinputpage.h"
#include "ui_abstractdialogpage.h"

#include <QDialog>
#include <QDebug>


uint32_t windowWidth = 100;
float offsetAverage = 0.5;

uint16_t dVolume = 5;
float Po = 19.529;
float Pk = 33.673;


VolumeInputPage::VolumeInputPage(QWidget *parent, uint8_t type) :
    AbstractDialogPage(parent),
    ui(new Ui::VolumeInputPage)
{
    ui->setupUi(AbstractDialogPage::ui->settingsPage);
    //spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
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
    dVolume = new QLabel("Укажите объем введенного болюса:", this);
    QFont fontLabel = dVolume->font();
    fontLabel.setPixelSize(20);
    dVolume->setFont(fontLabel);

    inputValueLineEdit = new QLineEdit(this);
    QFont fontLineEdit = inputValueLineEdit->font();
    fontLineEdit.setPixelSize(15);
    inputValueLineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    inputValueLineEdit->setFont(fontLineEdit);

    point0Label = new QLabel("Укажите координату x точки P0:", this);
    point0Label->setFont(fontLabel);

    point0LineEdit = new QLineEdit(this);
    point0LineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    point0LineEdit->setFont(fontLineEdit);

    point1Label = new QLabel("Укажите координату x точки Pk:", this);
    point1Label->setFont(fontLabel);

    point1LineEdit = new QLineEdit(this);
    point1LineEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
    point1LineEdit->setFont(fontLineEdit);

    spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->leftLayout->addSpacerItem(spacer);
    ui->leftLayout->addWidget(dVolume);
    ui->leftLayout->addWidget(inputValueLineEdit);
    ui->leftLayout->addWidget(point0Label);
    ui->leftLayout->addWidget(point0LineEdit);
    ui->leftLayout->addWidget(point1Label);
    ui->leftLayout->addWidget(point1LineEdit);
    spacer = nullptr;
    spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->leftLayout->addSpacerItem(spacer);
    spacer = nullptr;
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

    spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->leftLayout->addSpacerItem(spacer);
    spacer = nullptr;
    ui->leftLayout->addWidget(windowWidthLabel);
    ui->leftLayout->addWidget(windowWidthLineEdit);
    ui->leftLayout->addWidget(offsetAverageLabel);
    ui->leftLayout->addWidget(offsetAverageLineEdit);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    ui->leftLayout->addSpacerItem(spacer);
    spacer = nullptr;
}

#define DELITEM(_item) if(_item != nullptr)\
{\
    delete _item;\
    _item = nullptr;\
}

VolumeInputPage::~VolumeInputPage()
{
    DELITEM(dVolume);
    DELITEM(inputValueLineEdit);
    DELITEM(windowWidthLabel);
    DELITEM(windowWidthLineEdit);
    DELITEM(offsetAverageLabel);
    DELITEM(offsetAverageLineEdit);
    //DELITEM(spacer);
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
        if (typeOfValues == 1)
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
    if (typeOfValues == 1)
    {
    startValueWW = windowWidthLineEdit->text().toUInt();
    startValueOA = offsetAverageLineEdit->text().toFloat();
    }
}
