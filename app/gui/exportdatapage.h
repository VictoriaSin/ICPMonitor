#ifndef EXPORTDATAPAGE_H
#define EXPORTDATAPAGE_H

#include "abstractdialogpage.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QTimer>

struct dirsItem
{
    QLabel *label;
    QCheckBox *checkBox;
};

namespace Ui {
class ExportDataPage;
}
class MessageDialog;
class ExportDataPage : public AbstractDialogPage
{
    Q_OBJECT

public:
    explicit ExportDataPage(QWidget *parent = nullptr);
    ~ExportDataPage();

    void scaleFont(float scaleFactor) override;
    void installController(MonitorController *controller) override;
    void resetLayout();
private:
    Ui::ExportDataPage *ui;
    void clearLayout();
    //QVector<dirsItem> dirsVector;
    dirsItem *dirsVector{nullptr};
    uint arrSize = 0;
    QLabel* dirLabel {nullptr};
    QCheckBox *checkBox {nullptr};
    QString part_ForSave;
    QTimer flashTimer;
public slots:
    void retranslate() override;
    void done(int exodus) override;
protected:
    void showEvent(QShowEvent *event) override;
    QGridLayout *gridLayout {nullptr};
    void deleteItemDir(uint number);
    MessageDialog *mDeleteDirsDialog {nullptr};
    QPushButton *AcceptButton {nullptr};
    QPushButton *CancelButton {nullptr};
    bool isFlashAvailable {false};

protected slots:
    void selectAll();
    void clearSelection();
    void exportData();
    void deleteDirs();
    void findFlash();
};

#endif // EXPORTDATAPAGE_H
