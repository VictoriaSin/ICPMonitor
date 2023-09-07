#ifndef LANGUAGEPAGE_H
#define LANGUAGEPAGE_H

#include "abstractdialogpage.h"
#include "controller/controller_enums.h"

namespace Ui {
class LanguagePage;
}

class LanguagePage : public AbstractDialogPage
{
    Q_OBJECT

public:
    explicit LanguagePage(QWidget *parent = nullptr);
    ~LanguagePage();

private:
    /*! Обновление информации на виджетах
        о текущем установленном языке
    */
    void updateCurrentAppLanguageOnWidgets();

private:
    Ui::LanguagePage *ui;

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

public slots:
    void retranslate() override;

    // AbstractDialogPage interface
public slots:
    void done(int exodus) override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

};

#endif // LANGUAGEPAGE_H
