#ifndef ANIMATEDTOOLBUTTON_H
#define ANIMATEDTOOLBUTTON_H

#include <QToolButton>

class AnimatedToolButton : public QToolButton
{
    Q_OBJECT

    /*! Картинка нажатой кнопки */
    QIcon mPressedIcon;

    /*! Картинка обычной кнопки */
    QIcon mDefaultIcon;

public:
    AnimatedToolButton(QWidget *parent = nullptr);
    ~AnimatedToolButton();

    void setIcon(const QIcon &defaultIcon, const QIcon &pressedIcon);

private slots:
    /*! Обработка нажатия кнопки */
    void pressedButton();

    /*! Обрабокта отпускания кнопки */
    void releasedButton();
};

#endif // ANIMATEDTOOLBUTTON_H
