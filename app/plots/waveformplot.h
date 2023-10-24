#ifndef WAVEFORMPLOT_H
#define WAVEFORMPLOT_H

#include "abstractcustomplot.h"
#include "global_structs.h"

//class MonitorController;

class WaveFormPlot : public AbstractCustomPlot
{
    Q_OBJECT

public:
    explicit WaveFormPlot(QWidget *parent = nullptr);
    ~WaveFormPlot();

    /*! Добавление данных для отображения на графике */
    void addDataOnGraphic(unsigned int  x, /*unsigned int*/float  y);//(const ComplexValue &complexVal);

    /*! Перевод */
    void retranslate();

    void changePenColor(QColor mColor);

//#ifdef QT_DEBUG
    uint64_t benchTime = 0;
    uint64_t avgBenchTime = 0;
    int benchCount = 0;
//#endif

private:
public:
    /*! График с текущими показаниями */
    QCPGraph *mMainGraph {nullptr};

    /*! График с прошедшими показаниями */
    QCPGraph *mHistGraph {nullptr};

    // временный график
    QCPGraph *mTempGraph {nullptr};
    QCPGraph *mHistTempGraph {nullptr};
    QCPGraph *mAmplitudePoints {nullptr};
private:

public:
    /*! Линия верхнего уровня тревоги */
    QCPItemLine *mUpperAlarmLimit {nullptr};

    /*! Линия нижнего уровня тревоги */
    QCPItemLine *mLowerAlarmLimit {nullptr};
private:
    /*! Время прихода последнего значения датчика */
    int64_t mPreviousSensorDataTime {0};

    /*! Суммарное время пришедших данных с датчика */
    float mSummarySensorDataTimePerXRange {0};

    /*! Толщина линии графика с текущими показаниями */
    float mThicknessOfMainGraph {1.7};

    /*! Толщина линии графика с прошедшими показаниями */
    float mThicknessOfHistGraph {1};
    uint8_t type; //0-wave 1-compliance

public slots:
    /*! Установка линии верхнего уровня тревоги */
    void setUpperAlarmLevelLine(float upperAlarmLimit);

    /*! Установка линии нижнего уровня тревоги */
    void setLowerAlarmLevelLine(float lowerAlarmLimit);

    /*! Включение/Выключение верхнего уровня тревоги */
    void enableUpperAlarm(bool enable);

    /*! Включение/Выключение нижнего уровня тревоги */
    void enableLowerAlarm(bool enable);

    /*! Проверка оси X на вхождение в интервал*/
    //void checkXAxisInterval(const QCPRange &range);

    // AbstractCustomPlot interface

public:
    void resetGraph() override;
    uint graphRangeSize;//xAxis->range().size()*1000;
    uint graphCurrentMaxRange;
    uint graphMinus;
    void scaleFont(float scaleFactor) override;
    void setType(uint8_t newType);
    void addAvgDataOnGraphic(unsigned int x, float y);
    void addComplianceDataOnGraphic(unsigned int x, float y);
};

#endif // WAVEFORMPLOT_H
