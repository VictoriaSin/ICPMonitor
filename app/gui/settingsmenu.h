#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include "abstractmenupage.h"

class DateTimePage;
class LanguagePage;
class TechnicalAccessPage;

class SettingsMenu : public AbstractMenuPage
{
    Q_OBJECT

    DateTimePage *mDateTimePage {nullptr};
    LanguagePage *mLanguagePage {nullptr};
    TechnicalAccessPage *mTechnicalAccessPage {nullptr};

    /*! ID кнопок для доступа к ним */
    const int IDDateTimeButton {0};
    const int IDLanguageButton {1};
    const int IDTechnicalAccessButton {2};

public:
    explicit SettingsMenu(QWidget *parent = nullptr);

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

public slots:
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *) override;
};

#endif // SETTINGSMENU_H
