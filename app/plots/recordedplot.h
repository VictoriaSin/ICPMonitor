#ifndef RECORDEDPLOT_H
#define RECORDEDPLOT_H

#include "abstractcustomplot.h"
#include "global_structs.h"

class MarkItem;
class LabelManager;
class TextEditDialog;
class MonitorController;

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
    /*! Менеджер меток */
    //LabelManager *mLabelManager {nullptr};

    bool event(QEvent *event) override;
    explicit RecordedPlot(QWidget *parent = nullptr);
    ~RecordedPlot();



    double mCurrentMaxXRange {60};
    double mCurrentMaxYRange {60};

    /*! Добавление данных для отображения на графике */
    void saveDataForGraphic(const ComplexValue &complexVal);

    /*! Добавление данных для отображения на графике */
    void addDataOnGraphic();

    /*! Установка менеджера меток */
    //void setLabelManager(LabelManager *labelManager);

    /*! Перевод */
    void retranslate();

//#ifdef QT_DEBUG
    uint64_t benchTime = 0;
    uint64_t avgBenchTime = 0;
    int benchCount = 0;
//#endif

private:
    u32 pointStart;
    u32 pointStop;
    bool labelMoved;
    QCPGraph *mMainGraph {nullptr};

    QCPGraph *mIntervalFirst {nullptr};
    QCPGraph *mIntervalSecond {nullptr};
    /*! Толщина линии графика с текущими показаниями */
    double mThicknessOfMainGraph {2};

    /*! Контейнер для точек графика*/
    QVector<QPair<double, double>> mRecordedData;

    /*! Время прихода последнего значения датчика */
    int64_t mPreviousSensorDataTime {0};

    /*! Суммарное время пришедших данных с датчика */
    double mSummarySensorDataTimePerXRange {0};

public:
    void scaleFont(double scaleFactor) override;
    void resetGraph() override;

    void addInterval(uint8_t num, QColor color);
protected:
    bool editLabel(QMouseEvent *mouseEvent);
    bool editInterval(QMouseEvent *mouseEvent);
    bool editAxisRange(QMouseEvent *mouseEvent);
protected slots:
    /*! Обработка нажатий по элементам графика */
    void itemClicked(QCPAbstractItem *item, QMouseEvent *event);

private slots:
    /*! Проверка оси X на вхождение в интервал*/
    void checkXAxisInterval(const QCPRange &range);
    void checkYAxisInterval(const QCPRange &range);
};

#endif // RECORDEDPLOT_H
