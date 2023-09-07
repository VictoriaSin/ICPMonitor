#ifndef ABSTRACTDIALOGPAGE_H
#define ABSTRACTDIALOGPAGE_H

#include "ipagewidget.h"

class MonitorController;
class QSvgWidget;

namespace Ui {
class AbstractDialogPage;
}

class AbstractDialogPage : public IPageWidget
{
    Q_OBJECT

    // Общий стиль для кнопок
    const QString ToolButtonStyleSheet;

public:
    explicit AbstractDialogPage(QWidget *parent = nullptr);
    ~AbstractDialogPage();

    void setUpperSvgIcon(const QString &path);
    void setUpperNamePageLabel(const QString &text);
    void setBottomInfoLabel(const QString &text);

    /*! Прячет кнопку возвращения назад. */
    void enableRejectButton(bool enable);

    /*! Показывает/Прячет кнопку. */
    void enableAcceptButton(bool enable);

    /*! Установка изображения для кнопки отменить */
    void setIconRejectButton(const QIcon &defaultIcon, const QIcon &pressedIcon);

    /*! Установка изображения для кнопки подтвердить */
    void setIconAcceptButton(const QIcon &defaultIcon, const QIcon &pressedIcon);

private:
    /*! Настройка кнопки подтвердить */
    void setupAcceptButton();

    /*! Настройка кнопки отменить */
    void setupRejectButton();

protected:
    Ui::AbstractDialogPage *ui;
    QSvgWidget *mSvgIcon {nullptr};


    // IPageWidget interface
public:
    void scaleFont(float scaleFactor) override;
    void installController(MonitorController *controller) override;
    MonitorController *mController {nullptr};
public slots:
    virtual void done(int exodus);
    virtual void accept();
    virtual void reject();

    // IPageWidget interface
public slots:
    virtual void retranslate() override;
};

#endif // ABSTRACTDIALOGPAGE_H
