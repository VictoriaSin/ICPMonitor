#include "abstractmenupage.h"
#include "ui_abstractmenupage.h"
#include "gui/gui_funcs.h"

AbstractMenuPage::AbstractMenuPage(QWidget *parent) :
    IPageWidget(parent)
  , ui(new Ui::AbstractMenuPage)
{
    ui->setupUi(this);
    connect(ui->backButton, &QToolButton::clicked, this, &IPageWidget::previousPage);
}

void AbstractMenuPage::installController(MonitorController *controller)
{
    mController = controller;
}

void AbstractMenuPage::scaleFont(float scaleFactor)
{
    for (int i = 0; i < ui->buttonArea->count(); ++i) {
        WFontScaling(ui->buttonArea->itemAt(i)->widget(), scaleFactor);
    }
}

void AbstractMenuPage::retranslate()
{
    ui->retranslateUi(this);
}

void AbstractMenuPage::addSettingButton(const QString &nameLabel, const QIcon &defaultIconPath, const QIcon &pressedIconPath, const QSize &btnSize, const QString &styleSheet, IPageWidget* iPage, int row, int col, int buttonId)
{
    addSettingButton(nameLabel, defaultIconPath, pressedIconPath, btnSize, styleSheet, [this, iPage](){emit changePage(iPage);}, row, col, buttonId);
}

void AbstractMenuPage::setupBackButton(const QIcon &defaultIcon, const QIcon &pressedIconPath, const QSize &btnSize, const QString &styleSheet)
{
    ui->backButton->setIcon(defaultIcon, pressedIconPath);
    ui->backButton->setIconSize(btnSize);
    ui->backButton->setStyleSheet(styleSheet);
}

void AbstractMenuPage::addSettingButton(const QString &nameLabel, const QIcon &defaultIconPath, const QIcon &pressedIconPath, const QSize &btnSize, const QString &styleSheet, const std::function<void()> &callback, int row, int col, int buttonId)
{
    AnimatedToolButton *toolButton = new AnimatedToolButton(this);
    toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButton->setIcon(defaultIconPath, pressedIconPath);
    toolButton->setIconSize(btnSize);
    toolButton->setText(nameLabel);
#ifdef PC_BUILD
    toolButton->setFont(QFont("Noto Sans", 15, 400));
#else
    toolButton->setFont(FontForText);
#endif
    toolButton->setStyleSheet(styleSheet);
    connect(toolButton, &QToolButton::clicked, this, callback);

    ui->buttonArea->addWidget(toolButton, row, col);
    mButtonGroup.addButton(toolButton, buttonId);
}
