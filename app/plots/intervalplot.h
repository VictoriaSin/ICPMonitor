#ifndef INTERVALPLOT_H
#define INTERVALPLOT_H

#include "abstractcustomplot.h"
#include "global_define.h"
extern uint8_t mCurrentIntervalNum;
class MarkItem;

class IntervalPlot : public AbstractCustomPlot
{
    Q_OBJECT
public:
    explicit IntervalPlot(QWidget *parent = nullptr);
    //~IntervalPlot();
    bool event(QEvent *event) override;
    void retranslate();
    void scaleFont(float scaleFactor) override;

    void setup(QPair<int, int> points, QColor color);
private:
public:
    QCPGraph *mMainGraph {nullptr};
    void setMarks();
    uint iterTemp;
signals:

};

#endif // INTERVALPLOT_H
