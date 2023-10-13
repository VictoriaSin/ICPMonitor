#ifndef EXPORTDATAPAGE_H
#define EXPORTDATAPAGE_H

#include "abstractdialogpage.h"
#include <QScrollArea>
#include <QGridLayout>

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
    void clearLayout(QLayout *layout);
public slots:
    void retranslate() override;
    void done(int exodus) override;
protected:
    void showEvent(QShowEvent *event) override;
    QScrollArea *scrollArea1 {nullptr};
    QWidget *widget {nullptr};
    QGridLayout *gridLayout {nullptr};
protected slots:
    void selectAll();
    void clearSelection();
    void exportData();
    void deleteDirs();

};

#endif // EXPORTDATAPAGE_H
