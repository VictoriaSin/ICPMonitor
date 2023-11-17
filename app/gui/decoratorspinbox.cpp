#include "gui/decoratorspinbox.h"
#include "gui/gui_funcs.h"

#include <QDebug>
#include <QPushButton>
#include <QSpinBox>
#include <QLayout>
#include <QLineEdit>
#include <QDateTimeEdit>

DecoratorSpinBox::DecoratorSpinBox(QWidget *parent) :
    QWidget(parent),
    mMinusButton(new QPushButton("-", this)),
    mPlusButton(new QPushButton("+", this)),
    mHBoxLayout(new QHBoxLayout(this))
{
    mMinusButton->setMinimumSize(QSize(50, 50));
    mPlusButton->setMinimumSize(QSize(50, 50));

    mHBoxLayout->setSpacing(0);
    mHBoxLayout->addWidget(mMinusButton);
    mHBoxLayout->addWidget(mPlusButton);
    qDebug("\033[34m>>DecoratorSpinBox::DecoratorSpinBox\033[0m");
}

DecoratorSpinBox::~DecoratorSpinBox()
{
  qDebug("\033[34m<<DecoratorSpinBox::~DecoratorSpinBox\033[0m");
}

QAbstractSpinBox* DecoratorSpinBox::currentSpinBox() const
{
    return mCurrentSpinBox;
}

bool DecoratorSpinBox::setSpinBox(QAbstractSpinBox *spinBox)
{
    // Если пришёл такой же спинбокс
    if (mCurrentSpinBox == spinBox) {
        return false;
    }

    // Отключаем сигналы для прошлого спинбокса
    if (mCurrentSpinBox) {
        disconnect(mMinusButton, &QPushButton::clicked, mCurrentSpinBox, &QAbstractSpinBox::stepDown);
        disconnect(mPlusButton, &QPushButton::clicked, mCurrentSpinBox, &QAbstractSpinBox::stepUp);
        mHBoxLayout->removeWidget(mCurrentSpinBox);
        mCurrentSpinBox->hide();
        mCurrentSpinBox->deleteLater();
    }

    mCurrentSpinBox = spinBox;

    if (mCurrentSpinBox) {
        mHBoxLayout->insertWidget(1, mCurrentSpinBox);
        mCurrentSpinBox->setParent(this);

        mCurrentSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons); // Отключаем отображение кнопок спинбокса
        mCurrentSpinBox->setAlignment(Qt::AlignHCenter); // Устанавливаем по центру
        mCurrentSpinBox->setMinimumHeight(mMinusButton->height()); // Минимальная высота для спинбокса на уровне кнопок

        // Увеличение и уменьшение значений спинбокса
        connect(mMinusButton, &QPushButton::clicked, mCurrentSpinBox, [this](){
            mCurrentSpinBox->stepDown();
            mCurrentSpinBox->findChild<QLineEdit *>()->deselect();
        });
        connect(mPlusButton, &QPushButton::clicked, mCurrentSpinBox, [this](){
            mCurrentSpinBox->stepUp();
            mCurrentSpinBox->findChild<QLineEdit *>()->deselect();
        });

        return true;
    }

    return false;
}

bool DecoratorSpinBox::setRange(float minimum, float maximum)
{
    if (!mCurrentSpinBox) {
        return false;
    }

    if (auto spinBox = qobject_cast<QSpinBox *>(mCurrentSpinBox)) {
        spinBox->setRange(minimum, maximum);
        return true;
    } else if (auto doubleSpinBox = qobject_cast<QDoubleSpinBox *>(mCurrentSpinBox)) {
        doubleSpinBox->setRange(minimum, maximum);
        return true;
    }

    return false;
}

bool DecoratorSpinBox::setValue(float value)
{
    if (!mCurrentSpinBox) {
        return false;
    }

    if (auto spinBox = qobject_cast<QSpinBox *>(mCurrentSpinBox)) {
        spinBox->setValue(value);
        return true;
    } else if (auto doubleSpinBox = qobject_cast<QDoubleSpinBox *>(mCurrentSpinBox)) {
        doubleSpinBox->setValue(value);
        return true;
    }

    return false;
}

float DecoratorSpinBox::value(bool *ok) const
{
    if (!mCurrentSpinBox) {
        return false;
    }

    if (auto spinBox = qobject_cast<QSpinBox *>(mCurrentSpinBox)) {
        *ok = true;
        return spinBox->value();
    } else if (auto doubleSpinBox = qobject_cast<QDoubleSpinBox *>(mCurrentSpinBox)) {
        *ok = true;
        return doubleSpinBox->value();
    }

    *ok = false;
    return 0;
}

bool DecoratorSpinBox::setTime(const QTime &time)
{
    if (auto dateTimeSpinBox = qobject_cast<QDateTimeEdit *>(mCurrentSpinBox)) {
        dateTimeSpinBox->setTime(time);
        return true;
    }

    return false;
}

bool DecoratorSpinBox::setDate(const QDate &date)
{
    if (auto dateTimeSpinBox = qobject_cast<QDateTimeEdit *>(mCurrentSpinBox)) {
        dateTimeSpinBox->setDate(date);
        return true;
    }

    return false;
}

QTime DecoratorSpinBox::time(bool *ok) const
{
    if (auto dateTimeSpinBox = qobject_cast<QDateTimeEdit *>(mCurrentSpinBox)) {
        *ok = true;
        return dateTimeSpinBox->time();
    }

    *ok = false;
    return QTime();
}

QDate DecoratorSpinBox::date(bool *ok) const
{
    if (auto dateTimeSpinBox = qobject_cast<QDateTimeEdit *>(mCurrentSpinBox)) {
        *ok = true;
        return dateTimeSpinBox->date();
    }

    *ok = false;
    return QDate();
}
