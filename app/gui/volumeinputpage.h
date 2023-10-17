#ifndef VOLUMEINPUTPAGE_H
#define VOLUMEINPUTPAGE_H

#include "../abstractdialogpage.h"
#include <QLabel>
#include <QLineEdit>



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

    void setupVolume();
    void setupParam();

    QLabel *dVolume {nullptr};
    QLineEdit *inputValueLineEdit {nullptr};
    QLabel *windowWidthLabel {nullptr};
    QLineEdit *windowWidthLineEdit {nullptr};
    QLabel *offsetAverageLabel {nullptr};
    QLineEdit *offsetAverageLineEdit {nullptr};

private:
    Ui::VolumeInputPage *ui;
    uint32_t startValueWW;
    float startValueOA;

public slots:
    void retranslate() override;
    void done(int exodus) override;

protected:
    //void showEvent(QShowEvent *event) override;
    bool checkValue(QString inputString);
    void showEvent(QShowEvent *event) override;
signals:
    void intervalParamUpdated();
};

#endif // VOLUMEINPUTPAGE_H
