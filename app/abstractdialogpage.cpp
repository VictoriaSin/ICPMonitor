#include "abstractdialogpage.h"
#include "ui_abstractdialogpage.h"
#include "gui/gui_funcs.h"

#include <QDialog>
#include <QSvgWidget>

AbstractDialogPage::AbstractDialogPage(QWidget *parent) :
    IPageWidget(parent),
    ToolButtonStyleSheet(readStyleSheetFile(":/styles/ToolButtons.qss")),
    ui(new Ui::AbstractDialogPage),
    mSvgIcon(new QSvgWidget(this))
{
    ui->setupUi(this);

    // Настройка кнопок
    setupAcceptButton();
    setupRejectButton();
}

AbstractDialogPage::~AbstractDialogPage()
{
    delete ui;
}

void AbstractDialogPage::setUpperSvgIcon(const QString &path)
{
    mSvgIcon->load(path);
}

void AbstractDialogPage::setUpperNamePageLabel(const QString &text)
{
    ui->namePageLabel->setText(text);
}

void AbstractDialogPage::setBottomInfoLabel(const QString &text)
{
    ui->infoLabel->setText(text);
}

void AbstractDialogPage::enableRejectButton(bool enable)
{
    enable ? ui->cancelButton->show() : ui->cancelButton->hide();
}

void AbstractDialogPage::enableAcceptButton(bool enable)
{
    enable ? ui->confirmButton->show() : ui->confirmButton->hide();
}

void AbstractDialogPage::setIconRejectButton(const QIcon &defaultIcon, const QIcon &pressedIcon)
{
    ui->cancelButton->setIcon(defaultIcon, pressedIcon);
}

void AbstractDialogPage::setIconAcceptButton(const QIcon &defaultIcon, const QIcon &pressedIcon)
{
    ui->confirmButton->setIcon(defaultIcon, pressedIcon);
}

void AbstractDialogPage::setupAcceptButton()
{
    ui->confirmButton->setIcon(QIcon(":/icons/accept.svg"), QIcon(":/icons/accept_pressed.svg"));
    ui->confirmButton->setIconSize(QSize(100 ,100));
    ui->confirmButton->setStyleSheet(ToolButtonStyleSheet);
    connect(ui->confirmButton, &QToolButton::clicked, this, &AbstractDialogPage::accept);
}

void AbstractDialogPage::setupRejectButton()
{
    ui->cancelButton->setIcon(QIcon(":/icons/reject.svg"), QIcon(":/icons/reject_pressed.svg"));
    ui->cancelButton->setIconSize(QSize(100 ,100));
    ui->cancelButton->setStyleSheet(ToolButtonStyleSheet);
    connect(ui->cancelButton, &QToolButton::clicked, this, &AbstractDialogPage::reject);
}

void AbstractDialogPage::scaleFont(float scaleFactor)
{
    WFontScaling(ui->namePageLabel, scaleFactor);
    WFontScaling(ui->infoLabel, scaleFactor);
}

void AbstractDialogPage::installController(MonitorController *controller)
{
    mController = controller;
}

void AbstractDialogPage::done(int exodus)
{
    emit previousPage();
}

void AbstractDialogPage::accept()
{
    done(QDialog::Accepted);
}

void AbstractDialogPage::reject()
{
    done(QDialog::Rejected);
}

void AbstractDialogPage::retranslate()
{
    ui->retranslateUi(this);
}
