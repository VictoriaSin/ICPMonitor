#ifndef VOLUMEINPUTPAGE_H
#define VOLUMEINPUTPAGE_H

#include "../abstractdialogpage.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>


namespace Ui {
class VolumeInputPage;
}


class VolumeInputPage : public AbstractDialogPage
{
    Q_OBJECT

public:
    explicit VolumeInputPage(QWidget *parent = nullptr, uint8_t type = 0);
    //void scaleFont(float scaleFactor) override;
    ~VolumeInputPage();

    void setupVolume();
    void setupParam();

    QLabel *dVolume {nullptr};
    QLineEdit *inputValueLineEdit {nullptr};
    QLabel *point0Label {nullptr};
    QLineEdit *point0LineEdit {nullptr};
    QLabel *point1Label {nullptr};
    QLineEdit *point1LineEdit {nullptr};

    QLabel *windowWidthLabel {nullptr};
    QLineEdit *windowWidthLineEdit {nullptr};
    QLabel *offsetAverageLabel {nullptr};
    QLineEdit *offsetAverageLineEdit {nullptr};
    QSpacerItem *spacer {nullptr};

private:
    Ui::VolumeInputPage *ui;
    uint32_t startValueWW;
    float startValueOA;
    uint8_t typeOfValues; // 0- dV 1-avg

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
