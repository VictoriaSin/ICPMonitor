#ifndef ABSTRACTCUSTOMPLOT_H
#define ABSTRACTCUSTOMPLOT_H

#include "../app/plots/qcustomplot.h"
//#include "currentgraphsarea.h"
#include "controller/labels/labelmanager.h"
#include "../app/global_define.h"


class MonitorController;
class MarkItem;
class LabelMarkItem;

extern QVector<LabelMarkItem *> mLabelItemsContainer;

extern MarkItem * mIntervalsContainer[4];
extern uint8_t mCurrentIntervalNum;

extern bool isFluidIntervalCreating;
extern uint8_t mFluidMarksCounter;
extern MarkItem* mFluidMarkContainer[2];

class AbstractCustomPlot : public QCustomPlot
{
public:
    /*! Реализованные типы графиков
        WaveFormPlot - Основной график
        RecordedGraph - Записанный график
     */
    enum GraphType : uint8_t {
        WaveFormGraph,
        RecordedGraph,
        IntervalGraph
    };
    //bool labelMoved = false;

    AbstractCustomPlot(GraphType type, QWidget *parent = nullptr);

    bool event(QEvent *event) override;

    /*! Установка диапазона оси X (нижней) */
    QPair<float, float> setXRange(float lower, float upper);

    /*! Установка диапазона оси Y (левой) */
    void setYRange(float lower, float upper);

    /*! Тип графика */
    GraphType type() const;

    /*! Включение/Выключение адаптивной выборки меток */
    void setAdaptiveSamplingLabelItems(bool enabled);

    /*! Устанавливает элементы, которые нужно оптимизировать (не берёт во владение)
        false - если у QCustomPlot нет хотя бы одного элмента из списка;
        true - успешное добавление.
    */
    bool setOptimizationLabelItems(const QList<MarkItem *> items);

    /*! Добавляет элемент, который нужно оптимизировать (не берёт во владение)
        false - если у QCustomPlot нет этого элемента;
        true - успешное добавление.
    */
    bool addOptimizationLabelItem(MarkItem *item);

    /*! Возвращает элемент, подлежащий оптимизации, по индексу.
        Если элемента нет, возвращает nullptr.
    */
    MarkItem *optimizationLabelItem(int index) const;

    /*! Возвращает последний добавленный элемент.
        Если элемента нет, возвращает nullptr.
    */
    MarkItem *optimizationLabelItem() const;

    /*! Удаляет элемент из списка элементов, подлежащих оптимизации */
    bool removeOptimizationLabelItem(MarkItem *item);

    /*! Удаляет элемент из списка элементов, подлежащих оптимизации */
    bool removeOptimizationLabelItem(int index);

    /*! Очищает контейнер элементов (не удаляет элементы), подлежащих оптимизации */
    void clearOtimizationLabelItems();

    /*! Есть ли элемент в списке элементов, подлежащих оптимизации
        false - нет элемента;
        true - есть элемент.
    */
    bool hasOptimizationLabelItem(MarkItem *item) const;

    /*! Кол-во элементов, подлежащих оптимизации */
    int optimizationLabelItemCount() const;

    /*! Увеличение шрифта на графике */
    virtual void scaleFont(float scaleFactor);

    /*! Сброс графика */
    virtual void resetGraph();

    /*! Оптимизирует отображение меток, скрывая те, которые наложились друг на друга */
    void labelItemsOptimization();

private:
    /*! Базовая настройка отрисовки декартовой системы координат */
    void settingUpCartesianCoordinates();

    /*! Настройка оси X (нижней) */
    void settingUpXAxis();

    /*! Настройка оси X (верхней) */
    void settingUpXAxis2();

    /*! Настройка оси Y (левой) */
    void settingUpYAxis();

    /*! Настройка оси Y (правой) */
    void settingUpYAxis2();

    /*! Базовая настройка сглаживания элементов декартовых координат */
    void settingAnalisingElements();

    u32 pointStart;
    u32 pointStop;
    bool labelMoved;
protected:
    //uint32_t pointStart;
    //uint32_t pointStop;

    //! Тип графика
    const GraphType mGraphType;

    //! Было ли нажатие на виджет двумя пальцами
    bool mDoublePointsTouch {false};

    /*! Чувствительность приближения */
    float scaleSensitivity {17.777};

    /*! Включает адаптивную выборку меток */
    bool mAdaptiveSamplingLabelItems {false};

    /*! Элементы, подлежащие оптимизации */
    QList<MarkItem *> mOptimizedItems;
#ifdef PC_BUILD
    /*! Коэффициент для рассчёта шага скрытия элементов */
    float mOptimizeLabelsKoeff {0.05};
    bool editLabel(QMouseEvent *mouseEvent);
    bool editInterval(QMouseEvent *mouseEvent);
    bool editFluidInterval(QMouseEvent *mouseEvent);
    bool editAxisRange(QMouseEvent *mouseEvent, float minX, float maxX, float maxY);
#else
    /*! Коэффициент для рассчёта шага скрытия элементов */
    float mOptimizeLabelsKoeff {0.025};
    bool editLabel(QTouchEvent *mouseEvent);
    bool editInterval(QTouchEvent *mouseEvent);
    bool editAxisRange(QTouchEvent *mouseEvent, float minX, float maxX, float maxY);
#endif


};

#endif // ABSTRACTCUSTOMPLOT_H
