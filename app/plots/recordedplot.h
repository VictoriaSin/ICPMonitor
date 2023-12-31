#ifndef RECORDEDPLOT_H
#define RECORDEDPLOT_H

#include "abstractcustomplot.h"
#include "global_structs.h"

class MarkItem;
class LabelManager;
class TextEditDialog;
class MonitorController;

extern bool isPlayRecord;
extern bool isDownloadGraph;
extern QFile mRawDataFile;


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
    //bool event(QEvent *event) override;
    explicit RecordedPlot(QWidget *parent = nullptr);
    ~RecordedPlot();

    /*! Добавление данных для отображения на графике */
    void saveDataForGraphic(unsigned int  x, unsigned int  y);//(const ComplexValue &complexVal);

    /*! Добавление данных для отображения на графике */
    void addDataOnGraphic();

    /*! Перевод */
    void retranslate();

    void scaleFont(float scaleFactor) override;
    void resetGraph() override;

    QPair<int, int> addInterval(uint8_t num, QColor color);
    //void downloadData(QByteArray *temp);


//#ifdef QT_DEBUG
    uint64_t benchTime = 0;
    uint64_t avgBenchTime = 0;
    int benchCount = 0;
//#endif

private:
    u32 pointStart;
    u32 pointStop;
    bool labelMoved;
public:
    QCPGraph *mMainGraph {nullptr};
    QCPGraph *mIntervalFirst {nullptr};
    QCPGraph *mIntervalSecond {nullptr};
    QCPGraph *mFluidInjection {nullptr};
private:
    /*! Толщина линии графика с текущими показаниями */
    float mThicknessOfMainGraph {1.7};
public:
    /*! Контейнер для точек графика*/
    QVector<QPair<float, float>> mRecordedData;
    void addFluidInterval();
private:
    /*! Время прихода последнего значения датчика */
    int64_t mPreviousSensorDataTime {0};

    /*! Суммарное время пришедших данных с датчика */
    float mSummarySensorDataTimePerXRange {0};

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
    //void itemClicked(QCPAbstractItem *item, QMouseEvent *event);
signals:
    void changeBtnIcon();

};

#endif // RECORDEDPLOT_H
