#ifndef VOLUMEINPUTPAGE_H
#define VOLUMEINPUTPAGE_H

#include "../abstractdialogpage.h"

namespace Ui {
class VolumeInputPage;
}

class VolumeInputPage : public AbstractDialogPage
{
    Q_OBJECT

public:
    explicit VolumeInputPage(QWidget *parent = nullptr);
    //void scaleFont(float scaleFactor) override;
    ~VolumeInputPage();

private:
    Ui::VolumeInputPage *ui;

public slots:
    void retranslate() override;
    void done(int exodus) override;

protected:
    //void showEvent(QShowEvent *event) override;
    bool checkValue();
};

#endif // VOLUMEINPUTPAGE_H
