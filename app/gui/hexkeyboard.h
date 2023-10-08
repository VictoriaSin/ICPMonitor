#ifndef HEXKEYBOARD_H
#define HEXKEYBOARD_H

#include <QWidget>

namespace Ui {
class HexKeyboard;
}

class HexKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit HexKeyboard(QWidget *parent = nullptr);
    ~HexKeyboard();

private:
    Ui::HexKeyboard *ui;

    void sendHexKeyboardEvent(int key, const QString& text = QString());
};

#endif // HEXKEYBOARD_H
