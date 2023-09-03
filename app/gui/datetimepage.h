#ifndef DATETIMEPAGE_H
#define DATETIMEPAGE_H

#include "abstractdialogpage.h"

namespace Ui {
class DateTimePage;
}

class DateTimePage : public AbstractDialogPage
{
    Q_OBJECT

public:
    explicit DateTimePage(QWidget *parent = nullptr);
    ~DateTimePage();

private:
    Ui::DateTimePage *ui;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

    // AbstractDialogPage interface
public slots:
    void done(int exodus) override;

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

public slots:
    void retranslate() override;

};

#endif // DATETIMEPAGE_H
