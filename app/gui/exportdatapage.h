#ifndef EXPORTDATAPAGE_H
#define EXPORTDATAPAGE_H

#include "abstractdialogpage.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>

struct dirsItem
{
    QLabel *label;
    QCheckBox *checkBox;
};

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
    void resetLayout();
private:
    Ui::ExportDataPage *ui;
    void clearLayout();
    //QVector<dirsItem> dirsVector;
    dirsItem *dirsVector;
    uint arrSize = 0;
public slots:
    void retranslate() override;
    void done(int exodus) override;
protected:
    void showEvent(QShowEvent *event) override;
    QGridLayout *gridLayout;
    void deleteItemDir(uint number);
protected slots:
    void selectAll();
    void clearSelection();
    void exportData();
    void deleteDirs();

};

#endif // EXPORTDATAPAGE_H
