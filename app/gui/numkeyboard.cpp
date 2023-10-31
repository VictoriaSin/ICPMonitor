#include "numkeyboard.h"
#include "ui_numkeyboard.h"

#include <QDebug>
#include <QKeyEvent>

NumKeyboard::NumKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NumKeyboard)
{
    ui->setupUi(this);

    connect(ui->zero, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_0, "0");
    });

    connect(ui->one, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_1, "1");
    });
    connect(ui->two, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_2, "2");
    });
    connect(ui->three, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_3, "3");
    });
    connect(ui->four, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_4, "4");
    });
    connect(ui->five, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_5, "5");
    });
    connect(ui->six, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_6, "6");
    });
    connect(ui->seven, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_7, "7");
    });
    connect(ui->eight, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_8, "8");
    });
    connect(ui->nine, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_9, "9");
    });
//    connect(ui->minus, &QToolButton::clicked, this, [this](){
//        sendNumKeyboardEvent(Qt::Key_Minus, "-");
//    });
    connect(ui->dot, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_Period, ".");
    });
    connect(ui->erase, &QToolButton::clicked, this, [this](){
        sendNumKeyboardEvent(Qt::Key_Backspace,  "\b");
    });

    // Для установки цвета фона виджета клавиатуры
    //setAttribute(Qt::WA_StyledBackground);
}

NumKeyboard::~NumKeyboard()
{
    delete ui;
}

void NumKeyboard::sendNumKeyboardEvent(int key, const QString &text)
{
    QKeyEvent ev(QEvent::KeyPress, key, Qt::KeypadModifier, text);
    QCoreApplication::sendEvent(QGuiApplication::focusObject(), &ev);
}

