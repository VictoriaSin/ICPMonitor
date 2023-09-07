#ifndef ABSTRACTMENUPAGE_H
#define ABSTRACTMENUPAGE_H

#include "ipagewidget.h"

#include <functional>

#include <QButtonGroup>

namespace Ui {
class AbstractMenuPage;
}

class AbstractMenuPage : public IPageWidget
{
    Q_OBJECT

    const QSize SizeButtonIcon = QSize(125, 125);

    const QFont FontForText = QFont("Sans Serif", 20);

public:
    AbstractMenuPage(QWidget *parent = nullptr);

    /*! Метод добавляет кнопку (ToolButton) на страницу при нажатии которой срабатывает переданный метод callback
        nameLabel - текст, располагаемый справа от кнопки
        icon - картинка, располагаемая на кнопке, слева от текста
        row/col - строка/столбец, в которую помещаем кнопку
        buttonId - id для кнопки (дальнейший доступ через mButtonGroup)
        callback - функция, выполняемая при нажатии на кнопку
     */
    void addSettingButton(const QString &nameLabel, const QIcon &defaultIconPath, const QIcon &pressedIconPath, const QSize &btnSize, const QString &styleSheet, const std::function<void()>& callback, int row, int col, int buttonId);

    /*! Перегруженный метод добавления кнопки (ToolButton) на страницу.
        nameLabel - текст, располагаемый справа от кнопки
        icon - картинка, располагаемая на кнопке, слева от текста
        row/col - строка/столбец, в которую помещаем кнопку
        buttonId - id для кнопки (дальнейший доступ через mButtonGroup)
        iPage - страница, которую необходимо открыть
     */
    void addSettingButton(const QString &nameLabel, const QIcon &defaultIconPath, const QIcon &pressedIconPath, const QSize &btnSize, const QString &styleSheet, IPageWidget *iPage, int row, int col, int buttonId);

    /*! Настройка кнопки назад */
    void setupBackButton(const QIcon &defaultIcon, const QIcon &pressedIconPath, const QSize &btnSize, const QString &styleSheet);

protected:
    Ui::AbstractMenuPage *ui;
    MonitorController *mController {nullptr};

    /*! Для доступа к кнопкам */
    QButtonGroup mButtonGroup;

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

    // IPageWidget interface
public slots:
    virtual void retranslate() override;
};

#endif // ABSTRACTMENUPAGE_H
