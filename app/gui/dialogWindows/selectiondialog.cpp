#include "selectiondialog.h"
#include "ui_selectiondialog.h"
#include "ui_abstractdialogwindow.h"
#include "gui/gui_funcs.h"

#include <QDebug>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>

SelectionDialog::SelectionDialog(QWidget *parent) :
    AbstractDialogWindow(parent),
    ui(new Ui::SelectionDialog),
    mContainerButtons(new QButtonGroup(this))
{
    ui->setupUi(AbstractDialogWindow::ui->widget);

    // Устанавливаем текущий стиль для нажатых тулбаттонов
    mCurrentHighlightToolButtonStyle = TemplateHighlightToolButtonStyle.arg("DC6E14");

    // Жирный шрифт для текста в ToolButtons
    mFontForText.setBold(true);

    // Создаём кнопки
    mAcceptButton = new QPushButton(this);
    mRejectButton = new QPushButton(this);

    // Добавляем кнопки на форму
    AbstractDialogWindow::ui->buttonLayout->addWidget(mAcceptButton);
    AbstractDialogWindow::ui->buttonLayout->addWidget(mRejectButton);

    // Действия по нажатию на кнопки подтвердить и отменить
    connect(mAcceptButton, &QPushButton::clicked, this, &SelectionDialog::accept);
    connect(mRejectButton, &QPushButton::clicked, this, &SelectionDialog::reject);

    // Клик по кнопке
    connect(mContainerButtons, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &SelectionDialog::setLastClickedButton);
    connect(mContainerButtons, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, QOverload<QAbstractButton *>::of(&SelectionDialog::highlightToolButton));
}

SelectionDialog::~SelectionDialog()
{
    delete ui;
}

void SelectionDialog::scaleFont(float scaleFactor)
{
    WFontScaling(ui->infoLabel, scaleFactor);
    FontScaling(mFontForText, scaleFactor);

    mAcceptButton->setFont(mFontForText);
    mRejectButton->setFont(mFontForText);
}

QToolButton *SelectionDialog::addToolButton(const QString &nameLabel, const QIcon &icon, const QSize &sizeIcon, int id, int row, int col)
{
    QToolButton *toolButton = new QToolButton(this);

    if (nameLabel.isEmpty()) {
        toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    } else {
        toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }

    addRadioOrToolButton(toolButton, nameLabel, icon, sizeIcon, id, row, col);

    return toolButton;
}

QRadioButton *SelectionDialog::addRadioButton(const QString &nameLabel, const QIcon &icon, const QSize &sizeIcon, int id, int row, int col)
{
    QRadioButton *radioButton = new QRadioButton(this);

    addRadioOrToolButton(radioButton, nameLabel, icon, sizeIcon, id, row, col);

    return radioButton;
}

QAbstractButton *SelectionDialog::addRadioOrToolButton(QAbstractButton *button, const QString &nameLabel, const QIcon &icon, const QSize &sizeIcon, int id, int row, int col)
{
    button->setIcon(icon);
    button->setIconSize(sizeIcon);
    button->setText(nameLabel);
    button->setFont(mFontForText);
    button->setStyleSheet(BaseStyleForButtons + mCurrentHighlightToolButtonStyle);

    ui->buttonArea->addWidget(button, row, col, Qt::AlignCenter);
    mContainerButtons->addButton(button, id);

    return button;
}

void SelectionDialog::resetSelection()
{
    resetHighlightToolButtons();
    mLastClikedButton = nullptr;
}

void SelectionDialog::resetHighlightToolButtons()
{
    for (auto &button : mContainerButtons->buttons()) {
        button->setStyleSheet(BaseStyleForButtons);
    }
}

void SelectionDialog::setLastClickedButton(QAbstractButton *button)
{
    mLastClikedButton = button;
}

void SelectionDialog::highlightToolButton(int id)
{
    highlightToolButton(mContainerButtons->button(id));
}

void SelectionDialog::highlightToolButton(QAbstractButton *button)
{
    resetHighlightToolButtons();
    if (button) {
        button->setStyleSheet(BaseStyleForButtons + mCurrentHighlightToolButtonStyle);
    }
}

const QButtonGroup *SelectionDialog::getButtons() const
{
    return mContainerButtons;
}

int SelectionDialog::getSelectedId() const
{
    return mContainerButtons->id(mLastClikedButton);
}

void SelectionDialog::setInfoText(const QString &info)
{
    ui->infoLabel->setText(info);
}

void SelectionDialog::setToolButtonHighlightColor(const QString &hexColor)
{
    mCurrentHighlightToolButtonStyle = TemplateHighlightToolButtonStyle.arg(hexColor);
}

void SelectionDialog::removeButtons()
{
    auto listRB = mContainerButtons->buttons();
    const int countRB = listRB.size();

    for (int i = 0; i < countRB; ++i) {
        delete listRB.takeAt(0);
    }
}

QPushButton *SelectionDialog::getAcceptButton() const
{
    return mAcceptButton;
}

QPushButton *SelectionDialog::getRejectButton() const
{
    return mRejectButton;
}

void SelectionDialog::retranslate()
{
    mAcceptButton->setText(tr("Подтвердить"));
    mRejectButton->setText(tr("Отменить"));
}

void SelectionDialog::showEvent(QShowEvent *event)
{
    resetSelection();
    AbstractDialogWindow::showEvent(event);
}

void SelectionDialog::reject()
{
    resetSelection();
    AbstractDialogWindow::reject();
}
