#ifndef EXPORTDATAPAGE_H
#define EXPORTDATAPAGE_H

#include "abstractdialogpage.h"

namespace Ui {
class ExportDataPage;
}

class ExportDataPage : public AbstractDialogPage
{
    Q_OBJECT

public:
    explicit ExportDataPage(QWidget *parent = nullptr);
    ~ExportDataPage();

    void scaleFont(float scaleFactor) override;
    //void installController(MonitorController *controller) override;

private:
    Ui::ExportDataPage *ui;

public slots:
    void retranslate() override;
    void done(int exodus) override;
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // EXPORTDATAPAGE_H
