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
    void addDataOnGraphic(unsigned int  x, unsigned int  y);//(const ComplexValue &complexVal);

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
private:
    /*! Линия верхнего уровня тревоги */
    QCPItemLine *mUpperAlarmLimit {nullptr};

    /*! Линия нижнего уровня тревоги */
    QCPItemLine *mLowerAlarmLimit {nullptr};

    /*! Время прихода последнего значения датчика */
    int64_t mPreviousSensorDataTime {0};

    /*! Суммарное время пришедших данных с датчика */
    double mSummarySensorDataTimePerXRange {0};

    /*! Толщина линии графика с текущими показаниями */
    double mThicknessOfMainGraph {2};

    /*! Толщина линии графика с прошедшими показаниями */
    double mThicknessOfHistGraph {1};

    uint8_t mCurrentMaxXRange {60};

public slots:
    /*! Установка линии верхнего уровня тревоги */
    void setUpperAlarmLevelLine(double upperAlarmLimit);

    /*! Установка линии нижнего уровня тревоги */
    void setLowerAlarmLevelLine(double lowerAlarmLimit);

    /*! Включение/Выключение верхнего уровня тревоги */
    void enableUpperAlarm(bool enable);

    /*! Включение/Выключение нижнего уровня тревоги */
    void enableLowerAlarm(bool enable);

    /*! Проверка оси X на вхождение в интервал*/
    void checkXAxisInterval(const QCPRange &range);

    // AbstractCustomPlot interface
public:
    void resetGraph() override;

    void scaleFont(double scaleFactor) override;
};

#endif // WAVEFORMPLOT_H
