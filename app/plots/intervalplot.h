#ifndef INTERVALPLOT_H
#define INTERVALPLOT_H

#include "abstractcustomplot.h"
class MarkItem;

class IntervalPlot : public AbstractCustomPlot
{
    Q_OBJECT
public:
    explicit IntervalPlot(QWidget *parent = nullptr);
    //~IntervalPlot();
    bool event(QEvent *event) override;
    void retranslate();
    void scaleFont(double scaleFactor) override;

    void setup(QPair<int, int> points, QColor color);
private:
public:
    QCPGraph *mMainGraph {nullptr};
signals:

};

#endif // INTERVALPLOT_H
