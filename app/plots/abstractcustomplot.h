#ifndef ABSTRACTCUSTOMPLOT_H
#define ABSTRACTCUSTOMPLOT_H

#include "plots/qcustomplot.h"
//#include "currentgraphsarea.h"
#include "controller/labels/labelmanager.h"
#include "global_define.h"


class MonitorController;
class MarkItem;
class LabelMarkItem;

extern QVector<LabelMarkItem *> mLabelItemsContainer;

extern MarkItem * mIntervalsContainer[4];

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
    QPair<double, double> setXRange(double lower, double upper);

    /*! Установка диапазона оси Y (левой) */
    void setYRange(double lower, double upper);

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
    virtual void scaleFont(double scaleFactor);

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

protected:
    //uint32_t pointStart;
    //uint32_t pointStop;

    //! Тип графика
    const GraphType mGraphType;

    //! Было ли нажатие на виджет двумя пальцами
    bool mDoublePointsTouch {false};

    /*! Чувствительность приближения */
    double scaleSensitivity {17.777};

    /*! Включает адаптивную выборку меток */
    bool mAdaptiveSamplingLabelItems {false};

    /*! Элементы, подлежащие оптимизации */
    QList<MarkItem *> mOptimizedItems;
#ifdef PC_BUILD
    /*! Коэффициент для рассчёта шага скрытия элементов */
    double mOptimizeLabelsKoeff {0.05};
#else
    /*! Коэффициент для рассчёта шага скрытия элементов */
    double mOptimizeLabelsKoeff {0.025};
#endif
};

#endif // ABSTRACTCUSTOMPLOT_H
