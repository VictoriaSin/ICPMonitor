#ifndef DRAWGRAPHS_H
#define DRAWGRAPHS_H

#include <QObject>
#include <QThread>

//class CurrentGraphsArea;

class DrawGraphs : public QThread
{
    Q_OBJECT
public:
    DrawGraphs();
    ~DrawGraphs();

    volatile bool isStopped;
    volatile bool isRunning;
protected:
    void run();
    uint8_t buffSizeAvg = 20;
    uint8_t currIndex;
    uint32_t sum;
    uint32_t pointTime;
    //uint16_t *mData_1 {nullptr};
    //uint16_t *mData_2 {nullptr};
    float mData_2;
    uint32_t globalCount;
};


#endif // DRAWGRAPHS_H



