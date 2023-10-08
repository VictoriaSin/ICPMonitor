#include "hexkeyboard.h"
#include "ui_hexkeyboard.h"

#include <QKeyEvent>
#include <QLineEdit>
HexKeyboard::HexKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HexKeyboard)
{

    this->setParent(parentWidget()->parentWidget());
    ui->setupUi(this);
    connect(ui->characterA, &QToolButton::clicked, this, [this](){
        sendHexKeyboardEvent(Qt::Key_A, "A");
    });
    connect(ui->characterB, &QToolButton::clicked, this, [this](){
        sendHexKeyboardEvent(Qt::Key_B, "B");
    });
    connect(ui->characterC, &QToolButton::clicked, this, [this](){
        sendHexKeyboardEvent(Qt::Key_C, "C");
    });
    connect(ui->characterD, &QToolButton::clicked, this, [this](){
        sendHexKeyboardEvent(Qt::Key_D, "D");
    });
    connect(ui->characterE, &QToolButton::clicked, this, [this](){
        sendHexKeyboardEvent(Qt::Key_E, "E");
    });
    connect(ui->characterF, &QToolButton::clicked, this, [this](){
        sendHexKeyboardEvent(Qt::Key_F, "F");
    });
    connect(ui->characterX, &QToolButton::clicked, this, [this](){
        sendHexKeyboardEvent(Qt::Key_X, "x");
    });

    // Для установки цвета фона виджета клавиатуры
    //setAttribute(Qt::WA_StyledBackground);
}

HexKeyboard::~HexKeyboard()
{
    delete ui;
}

void HexKeyboard::sendHexKeyboardEvent(int key, const QString &text)
{
    QKeyEvent ev(QEvent::KeyPress, key, Qt::KeypadModifier, text);
    QCoreApplication::sendEvent(QGuiApplication::focusObject(), &ev);
    //QLineEdit* reg = (QLineEdit*)QGuiApplication::focusObject();
    //reg->insert(text);
}
