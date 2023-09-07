#include "animatedtoolbutton.h"

AnimatedToolButton::AnimatedToolButton(QWidget *parent) :
    QToolButton(parent)
{
    mDefaultIcon = QToolButton::icon();
    mPressedIcon = mDefaultIcon;

    //Анимация при нажатии на кнопку
    connect(this, &QAbstractButton::pressed, this, &AnimatedToolButton::pressedButton);
    connect(this, &QAbstractButton::released, this, &AnimatedToolButton::releasedButton);
}

AnimatedToolButton::~AnimatedToolButton()
{

}

void AnimatedToolButton::setIcon(const QIcon &defaultIcon, const QIcon &pressedIcon)
{
    mDefaultIcon = defaultIcon;
    mPressedIcon = pressedIcon;
    QToolButton::setIcon(defaultIcon);
}

void AnimatedToolButton::pressedButton()
{
    QToolButton::setIcon(mPressedIcon);
}

void AnimatedToolButton::releasedButton()
{
    QToolButton::setIcon(mDefaultIcon);
}
