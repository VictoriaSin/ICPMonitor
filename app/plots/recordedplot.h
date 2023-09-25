#ifndef RECORDEDPLOT_H
#define RECORDEDPLOT_H

#include "abstractcustomplot.h"
#include "global_structs.h"

class MarkItem;
class LabelManager;
class TextEditDialog;
class MonitorController;

extern bool isPlayRecord;
extern double mCurrentMaxYRange;
class RecordedPlot : public AbstractCustomPlot
{
    Q_OBJECT

    /*! Окно для просмотра и редактирования информации о метах */
    TextEditDialog *mTextEditDialog {nullptr};

    /*! Шаблон для заполнения информации о метке */
    const QString TemplateInfoAboutLabel = "%1: %2\n"
                                           "%3: %4\n"
                                           "%5: %6\n"
                                           "%7: %8\n"
                                           "%9:";

public:
    bool event(QEvent *event) override;
    explicit RecordedPlot(QWidget *parent = nullptr);
    ~RecordedPlot();

    double mCurrentMaxXRange {60};

    /*! Добавление данных для отображения на графике */
    void saveDataForGraphic(unsigned int  x, unsigned int  y);//(const ComplexValue &complexVal);

    /*! Добавление данных для отображения на графике */
    void addDataOnGraphic();

    /*! Перевод */
    void retranslate();

    void scaleFont(double scaleFactor) override;
    void resetGraph() override;

    QVector<QPair<double, double>> addInterval(uint8_t num, QColor color);
    void downloadData(QByteArray *temp);


//#ifdef QT_DEBUG
    uint64_t benchTime = 0;
    uint64_t avgBenchTime = 0;
    int benchCount = 0;
//#endif

    /*! Менеджер меток */
    //LabelManager *mLabelManager {nullptr};

    /*! Установка менеджера меток */
    //void setLabelManager(LabelManager *labelManager);

private:
    u32 pointStart;
    u32 pointStop;
    bool labelMoved;
public:
    QCPGraph *mMainGraph {nullptr};
    QCPGraph *mIntervalFirst {nullptr};
    QCPGraph *mIntervalSecond {nullptr};
private:
    /*! Толщина линии графика с текущими показаниями */
    double mThicknessOfMainGraph {2};
public:
    /*! Контейнер для точек графика*/
    QVector<QPair<double, double>> mRecordedData;
private:
    /*! Время прихода последнего значения датчика */
    int64_t mPreviousSensorDataTime {0};

    /*! Суммарное время пришедших данных с датчика */
    double mSummarySensorDataTimePerXRange {0};

protected:
    bool editLabel(QMouseEvent *mouseEvent);
    bool editInterval(QMouseEvent *mouseEvent);
    bool editAxisRange(QMouseEvent *mouseEvent);

    bool editLabel(QTouchEvent *touchEvent);
    bool editInterval(QTouchEvent *touchEvent);
    bool editAxisRange(QTouchEvent *touchEvent);

public slots:
    void animateGraphic(int timerDelaySec);

protected slots:
    /*! Обработка нажатий по элементам графика */
    void itemClicked(QCPAbstractItem *item, QMouseEvent *event);
signals:
    void changeBtnIcon();

};

#endif // RECORDEDPLOT_H
