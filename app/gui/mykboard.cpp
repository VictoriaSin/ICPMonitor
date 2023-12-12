#include "mykboard.h"
#include "ui_mykboard.h"
#include <QPushButton>
#include <QString>
#include <QKeyEvent>
#include <QDebug>

#define K_INIT_CONNECT(NAME)\
connect(ui->butKey_##NAME, &QPushButton::clicked, this, &myKBoard::butClick);\
ui->butKey_##NAME->setObjectName(QString::number(butKey_##NAME))

    myKBoard::myKBoard(QWidget *parent) :  QWidget(parent),  ui(new Ui::myKBoard)
{
  ui->setupUi(this);
  setStyleSheet("QPushButton {  "
                "border: 1px solid #555; "
                "background-color: white;"
                "border-radius: 5px;}"
                "QPushButton::pressed {background-color: #40dfff;}");

  ui->lineEdit->hide();
  ui->butKey_Exit->hide();
  K_INIT_CONNECT(1_1);
  K_INIT_CONNECT(1_2);
  K_INIT_CONNECT(1_3);
  K_INIT_CONNECT(1_4);
  K_INIT_CONNECT(1_5);
  K_INIT_CONNECT(1_6);
  K_INIT_CONNECT(1_7);
  K_INIT_CONNECT(1_8);
  K_INIT_CONNECT(1_9);
  K_INIT_CONNECT(1_10);
  K_INIT_CONNECT(1_11);
  K_INIT_CONNECT(1_12);

  K_INIT_CONNECT(2_1);
  K_INIT_CONNECT(2_2);
  K_INIT_CONNECT(2_3);
  K_INIT_CONNECT(2_4);
  K_INIT_CONNECT(2_5);
  K_INIT_CONNECT(2_6);
  K_INIT_CONNECT(2_7);
  K_INIT_CONNECT(2_8);
  K_INIT_CONNECT(2_9);
  K_INIT_CONNECT(2_10);
  K_INIT_CONNECT(2_11);

  K_INIT_CONNECT(3_1);
  K_INIT_CONNECT(3_2);
  K_INIT_CONNECT(3_3);
  K_INIT_CONNECT(3_4);
  K_INIT_CONNECT(3_5);
  K_INIT_CONNECT(3_6);
  K_INIT_CONNECT(3_7);
  K_INIT_CONNECT(3_8);
  K_INIT_CONNECT(3_9);
  K_INIT_CONNECT(3_10);

  K_INIT_CONNECT(Reg);
  K_INIT_CONNECT(Space);
  K_INIT_CONNECT(Shift);
  K_INIT_CONNECT(Num);
  K_INIT_CONNECT(Exit);
  K_INIT_CONNECT(Enter);
  K_INIT_CONNECT(Back);

  mAnimation = new QPropertyAnimation(this, "");
  connect(mAnimation, &QPropertyAnimation::finished, this, &myKBoard::finishAnimation );
  ui->butKey_Exit ->setIcon(QIcon(":/icons/close.svg"));
  ui->butKey_Enter->setIcon(QIcon(":/icons/enter.svg"));
  ui->butKey_Back ->setIcon(QIcon(":/icons/backSpace.svg"));
  ui->butKey_Shift->setIcon(QIcon(":/icons/shift_2.svg"));

}

myKBoard::~myKBoard()
{
  delete ui;
}

void myKBoard::showExitBtn()
{
    ui->butKey_Exit->show();
}


void myKBoard::finishAnimation()
{
  emit finished(oldString);
  hide();
}

void myKBoard::changeLang()
{
#define SET_LETTER(NAME) if (stateShift == UPPER) { ui->butKey_##NAME->setText(letBase[butKey_##NAME][selectRowLang]);} else { ui->butKey_##NAME->setText(letBase[butKey_##NAME][selectRowLang].toLower());}
  uint8_t selectRowLang;
  if (stateNumeric == LOWER)
  {
    selectRowLang = 2;
  }
  else
  {
    if (curRegion == ENG)
    {
      selectRowLang = 1;
      ui->butKey_Space->setText("SPACE"); ui->butKey_Reg->setText("РУС");

    }
    else
    {
      selectRowLang = 0;
      ui->butKey_Space->setText("ПРОБЕЛ"); ui->butKey_Reg->setText("ENG");
    }
    if (stateShift == UPPER)
    { ui->butKey_Shift->setText("shift"); ui->butKey_Shift->setIcon(QIcon(":/icons/shift_2.svg"));}
    else { ui->butKey_Shift->setText("SHIFT"); ui->butKey_Shift->setIcon(QIcon(":/icons/shift_1.svg"));}
  }

  SET_LETTER(1_1);
  SET_LETTER(1_2);
  SET_LETTER(1_3);
  SET_LETTER(1_4);
  SET_LETTER(1_5);
  SET_LETTER(1_6);
  SET_LETTER(1_7);
  SET_LETTER(1_8);
  SET_LETTER(1_9);
  SET_LETTER(1_10);
  SET_LETTER(1_11);
  SET_LETTER(1_12);

  SET_LETTER(2_1);
  SET_LETTER(2_2);
  SET_LETTER(2_3);
  SET_LETTER(2_4);
  SET_LETTER(2_5);
  SET_LETTER(2_6);
  SET_LETTER(2_7);
  SET_LETTER(2_8);
  SET_LETTER(2_9);
  SET_LETTER(2_10);
  SET_LETTER(2_11);

  SET_LETTER(3_1);
  SET_LETTER(3_2);
  SET_LETTER(3_3);
  SET_LETTER(3_4);
  SET_LETTER(3_5);
  SET_LETTER(3_6);
  SET_LETTER(3_7);
  SET_LETTER(3_8);
  SET_LETTER(3_9);
  SET_LETTER(3_10);
}

void myKBoard::butClick()
{
  QPushButton *senderButton = qobject_cast<QPushButton *>(this->sender());
  int numButton = senderButton->objectName().toInt();

  switch (numButton)
  {
    case butKey_Reg:
    {
      if (curRegion == RUS) { curRegion = ENG; } else { curRegion = RUS; }
      changeLang();
      return;
    }
    case butKey_Shift:
    {
      if (stateShift == UPPER) { stateShift = LOWER; }  else { stateShift = UPPER; }
      changeLang();
      return;
    }
    case butKey_Num:
    {
      if (stateNumeric == UPPER)
      {
        ui->butKey_Reg->setEnabled(false);
        ui->butKey_Shift->setEnabled(false);
        ui->butKey_Num->setText("ABC");
        stateNumeric = LOWER;
      }
      else
      {
        ui->butKey_Reg->setEnabled(true);
        ui->butKey_Shift->setEnabled(true);
        ui->butKey_Num->setText("123");
        stateNumeric = UPPER;
      }
      changeLang();
      return;
    }
    case butKey_Exit:
    {
      animateWidget();
      return;
    }
    case butKey_Enter:
    {
      emit sendChar("enter");
      return;
    }
    case butKey_Back:
    {
      emit sendChar("backspace");
      return;
    }
  }

  QString insertChar;
  if (numButton == butKey_Space) { insertChar = " "; }
  else
  {
    if (stateNumeric == LOWER) insertChar = letBase[numButton][2];
    else
    {
      if (stateShift == UPPER) { insertChar = letBase[numButton][curRegion]; }
      else { insertChar = letBase[numButton][curRegion].toLower(); }
    }
  }
  emit sendChar(insertChar);

}

void myKBoard::animateWidget()
{
  mAnimation->setDuration(500);
  mAnimation->setStartValue(pos().y());
  mAnimation->setEndValue(pos().y() + rect().height() + 1);
  connect(mAnimation, &QPropertyAnimation::valueChanged, [this](const QVariant& value)
          {
            this->move(3+6,rect().y()+value.toInt());
          });
  mAnimation->start();  
}
