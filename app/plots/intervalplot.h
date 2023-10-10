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
    ~IntervalPlot();
    void retranslate();
    void scaleFont(float scaleFactor) override;

    void setup(QPair<int, int> points, QColor color);
private:
    uint maxBuffSizeAvg;
    uint16_t firstBuffPointer;
    uint16_t lastBuffPointer;
    uint sum;//double sum;
    uint16_t cnt;
    uint16_t *CurrDataForAverage;
    QCPGraph *mAvgGraph;
    QCPGraph* average;
    QCPGraph* substract;
    QCPGraph* amplitude;
    QCPGraph* filter;
public:
    void averagePlot(/*_mSPIData temp*/);
public:
    QCPGraph *mMainGraph {nullptr};
    void setMarks();
    uint iterTemp;

    void Compliance();
};

#endif // INTERVALPLOT_H
