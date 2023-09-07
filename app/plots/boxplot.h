#ifndef BOXPLOT_H
#define BOXPLOT_H

#include "abstractcustomplot.h"

class ComplexValue;

class BoxPlot : public AbstractCustomPlot
{
    Q_OBJECT

    /*! Ящик с усами */
    QCPStatisticalBox *mBoxGraph {nullptr};

    /*! Коэффициент рассчёта ширини коробки */
    double mKoeffBoxWidth {18};

    /*! Коэффициент рассчёта ширини усов */
    double mKoeffWhiskerWidth {36};

public:
    BoxPlot(QWidget *parent = nullptr);
    ~BoxPlot();

    /*! Установка контейнера данных для отображения */
    void setDataContainer(QSharedPointer<QCPStatisticalBoxDataContainer> data);

    /*! Возврат контейнера с данными */
    QSharedPointer<QCPStatisticalBoxDataContainer> getDataContainer() const;

    /*! Настройка отрисовки коробки */
    void setupBoxDrawing(const QPen &frameBoxPen, const QBrush &boxBrush, double boxWidth);

    /*! Базовая настройка отрисовка коробки */
    void baseBoxDrawing();

    /*! Настройка отрисовки остова усов */
    void setupBackboneWhiskerDrawing(const QPen &backboneWhiskerPen);

    /*! Базовая настройка отрисовки остова усов */
    void baseBackboneWhiskerDrawing();

    /*! Настройка отрисовки усов */
    void setupWhiskerDrawing(const QPen &whiskerBarPen, double whiskerWidth);

    /*! Базовая настройка отрисовки усов */
    void baseWhiskerDrawing();

    /*! Настройка отрисовки линии медианы */
    void setupMedianDrawing(const QPen &medianPen);

    /*! Базовая настройка отрисовки линии медианы */
    void baseMedianPenDrawing();

    /*! Настройка отрисовки выбросов */
    void setupOutlierStyleDrawing(const QCPScatterStyle &scatterStyle);

    /*! Базовая настройка отрисовки выбросов */
    void baseOutlierStyleDrawing();

    /*! Масштабирование коробок */
    void scaleBoxesRelativeToDataRange();

    /*! Перевод */
    void retranslate();

    /*! Рассчёт коробчатой диаграммы
        Коробчатая диаграмма формируется слева направо. В последнюю коробку набираются
        оставшиеся данные.
        ВХОДЯЩИЕ ПАРАМЕТРЫ:
        data - указатель на данные (могут быть отсортированы);
        intervalSeconds - интервал, за который считается одна коробка;
        isSortedByTime - данные отсортированы по времени.
        ВЫХОДЯЩИЕ ПАРАМЕТРЫ:
        QCPStatisticalBoxDataContainer - контейнер с рассчитаными коробками
    */
    static QCPStatisticalBoxDataContainer calculateBoxGraph(std::vector<ComplexValue> *data, const QCPRange &xRange, int8_t countBox, bool isSortedByTime = false);

    /*! Расчёт коробки для коробчатой диаграммы
        Значение key необходимо вычислить самому.
        ВХОДЯЩИЕ ПАРАМЕТРЫ:
        data - указатель на данные (будут отсортированы по возрастанию значения, если удастся посчитать коробку);
        isOkey - удалось лы посчитать коробку;
        isSortedByValue - отсортировано ли по значению.
    */
    static QCPStatisticalBoxData calculateTheBox(std::vector<ComplexValue> *data, bool &isOkey, bool isSortedByValue = false);

    /*! Вычисляет границы по оси Y коробчатой диаграммы для размещения
        всех коробок с усами внутри видимой области и записывает их в lower и upper
    */
    static void arrangeAllTheBoxesWithWhiskersInside(QCPStatisticalBoxDataContainer *data, double &lower, double &upper);

    /*! Вычисляет границы по оси Y коробчатой диаграммы для размещения
        всех коробок внутри видимой области и записывает их в lower и upper
    */
    static void arrangeAllTheBoxesInside(QCPStatisticalBoxDataContainer *data, double &lower, double &upper);

    // AbstractCustomPlot interface
public:
    void scaleFont(double scaleFactor) override;
};

#endif // BOXPLOT_H
