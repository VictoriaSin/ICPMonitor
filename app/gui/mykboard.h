#ifndef MYKBOARD_H
#define MYKBOARD_H

#include <QPropertyAnimation>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class myKBoard; }
QT_END_NAMESPACE

class myKBoard : public QWidget
{
  Q_OBJECT
  QString letBase[39][3] =
    {
     {"Й", "Q", "1"},
     {"Ц", "W", "2"},
     {"У", "E", "3"},
     {"К", "R", "4"},
     {"Е", "T", "5"},
     {"Н", "Y", "6"},
     {"Г", "U", "7"},
     {"Ш", "I", "8"},
     {"Щ", "O", "9"},
     {"З", "P", "0"},
     {"Х", "[", "{"},
     {"Ъ", "]", "}"},

     {"Ф", "A", "@"},
     {"Ы", "S", "#"},
     {"В", "D", "="},
     {"А", "F", "_"},
     {"П", "G", "&&"},
     {"Р", "H", "-"},
     {"О", "J", "+"},
     {"Л", "K", "("},
     {"Д", "L", ")"},
     {"Ж", ":", "/"},
     {"Э", ";", "\\"},

     {"Я", "Z", "*"},
     {"Ч", "X", "\""},
     {"С", "C", "'"},
     {"М", "V", ":"},
     {"И", "B", ";"},
     {"Т", "N", "!"},
     {"Ь", "M", "?"},
     {"Б", "_", "|"},
     {"Ю", "-", "%"},
     {"/", "=", "$"},
  };

enum myBut
  {
    butKey_1_1,
    butKey_1_2,
    butKey_1_3,
    butKey_1_4,
    butKey_1_5,
    butKey_1_6,
    butKey_1_7,
    butKey_1_8,
    butKey_1_9,
    butKey_1_10,
    butKey_1_11,
    butKey_1_12,

    butKey_2_1,
    butKey_2_2,
    butKey_2_3,
    butKey_2_4,
    butKey_2_5,
    butKey_2_6,
    butKey_2_7,
    butKey_2_8,
    butKey_2_9,
    butKey_2_10,
    butKey_2_11,

    butKey_3_1,
    butKey_3_2,
    butKey_3_3,
    butKey_3_4,
    butKey_3_5,
    butKey_3_6,
    butKey_3_7,
    butKey_3_8,
    butKey_3_9,
    butKey_3_10,

    butKey_Space,
    butKey_Reg,
    butKey_Shift,
    butKey_Exit,
    butKey_Num,
    butKey_Enter,
    butKey_Back,
  };

public:
  explicit myKBoard(QWidget *parent = nullptr);
  ~myKBoard();
  void setString(const QString &newString);

private:
  void finishAnimation();
  QPropertyAnimation *mAnimation;
  QString oldString{};
  void changeLang();
  void butClick();
  const static uint8_t RUS {0};
  const static uint8_t ENG {1};
  const static uint8_t NUM {2};
  uint8_t curRegion{RUS};
  const static uint8_t UPPER {0};
  const static uint8_t LOWER {1};
  uint8_t stateShift{UPPER};
  uint8_t stateNumeric{UPPER};
public:
  void animateWidget();
  void showExitBtn();
signals:
  void finished(QString);
  void sendChar(QString);
private:
  Ui::myKBoard *ui;
};

#endif // MYKBOARD_H
