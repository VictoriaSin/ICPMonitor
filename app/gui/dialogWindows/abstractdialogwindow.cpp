#include "abstractdialogwindow.h"
#include "ui_abstractdialogwindow.h"
#include "gui/gui_funcs.h"

#include <QScreen>
#include <QDebug>

AbstractDialogWindow::AbstractDialogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AbstractDialogWindow)
{
    ui->setupUi(this);
}

AbstractDialogWindow::~AbstractDialogWindow()
{
    delete ui;
}

void AbstractDialogWindow::setCenter()
{
    updateGeometry();
    const auto &screen = QApplication::screenAt(QPoint(0, 0));
    this->move(screen->geometry().center() - this->rect().center());
}

void AbstractDialogWindow::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    setCenter();
}
