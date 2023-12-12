#include "patientinfodialog.h"
#include "ui_patientinfodialog.h"

#include <QPlainTextEdit>
#include <QDebug>
#include <QScreen>

PatientInfoDialog::PatientInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatientInfoDialog)
{
    ui->setupUi(this);
    ui->keyboard->hide();
    setWindowFlags(Qt::FramelessWindowHint);
    kboardBtnStatus = false;
    currLineEdit = nullptr;
    currInput = inputWidget::line;
    ui->nameLineEdit->installEventFilter(this);
    ui->surnameLineEdit->installEventFilter(this);
    ui->plainTextEdit->installEventFilter(this);
    connect(ui->keyboard, &myKBoard::sendChar, this, &PatientInfoDialog::printChar);
    connect(ui->acceptButton, &QPushButton::clicked, this, &PatientInfoDialog::acceptPatientInfo);
}

PatientInfoDialog::~PatientInfoDialog()
{
    delete currLineEdit;
    delete ui;
}

void PatientInfoDialog::on_openKeyboardButton_clicked()
{
    kboardBtnStatus = !kboardBtnStatus;
    if (kboardBtnStatus == true)
    {
        ui->keyboard->show();
        //ui->openKeyboardButton->hide();
        ui->openKeyboardButton->setText("Скрыть");
    }
    else
    {
        ui->keyboard->animateWidget();
        //ui->openKeyboardButton->show();
        ui->openKeyboardButton->setText("Изменить");
    }
}

void PatientInfoDialog::printChar(QString inputText)
{
    if (inputText == "backspace")
    {
        if (currInput == inputWidget::line)
        {
            currLineEdit->backspace();
        }
        else if (currInput == inputWidget::text) ui->plainTextEdit->textCursor().deletePreviousChar();
    }
    else if (inputText == "enter")
    {
        QDialog::accept();
        ui->openKeyboardButton->show();
    }
    else
    {
        if (currInput == inputWidget::line)
        {
            currLineEdit->insert(inputText);
        }
        else if (currInput == inputWidget::text) ui->plainTextEdit->insertPlainText(inputText);
    }
    ui->plainTextEdit->setFocus();
}

void PatientInfoDialog::setCenter()
{
    updateGeometry();
    const auto &screen = QApplication::screenAt(QPoint(0, 0));
    this->move(screen->geometry().center() - this->rect().center());
}

void PatientInfoDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    ui->keyboard->hide();
    ui->openKeyboardButton->show();
    setCenter();
}

void PatientInfoDialog::setLabel(QString newLabel)
{
    ui->label->setText(newLabel);
}

void PatientInfoDialog::setPatientDialog()
{
    ui->label->setText("Пациент");
    ui->plainTextEdit->hide();
}

void PatientInfoDialog::setInfoDialog()
{
    ui->label->setText("Информация");
    ui->nameLabel->hide();
    ui->surnameLabel->hide();
    ui->nameLineEdit->hide();
    ui->surnameLineEdit->hide();
}

bool PatientInfoDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        if (watched->metaObject()->className() == QLatin1String("QLineEdit"))
        {
            currLineEdit = (QLineEdit*) watched;
            currInput = inputWidget::line;
            currLineEdit->setFocus();
        }
        else if (watched->metaObject()->className() == QLatin1String("QPlainTextEdit"))
        {
            currInput = inputWidget::text;
            ui->plainTextEdit->setFocus();
        }
    }
    return false;
}

void PatientInfoDialog::acceptPatientInfo()
{
    if (currInput == inputWidget::line)
    {
        QString pName = ui->nameLineEdit->text(), pSurname = ui->surnameLineEdit->text();
        if (!checkInputData(pName) || !checkInputData(pSurname))
        {
            ui->label->setText("Некорректный ввод!");
            return;
        }
        else
        {
            dirName = pSurname + pName; //можно добавить проверку на лишние символы тут или в регулярке
        }
    }
    QDialog::accept();
}

bool PatientInfoDialog::checkInputData(QString inputParametr)
{
    QRegExp mRegExp("([A-Z][a-z]*)|([А-Я][а-я]*)");
    return mRegExp.exactMatch(inputParametr);
}

QString PatientInfoDialog::getDirName()
{
    return dirName;
}
