#ifndef STATISTICGRAPHSAREA_H
#define STATISTICGRAPHSAREA_H

#include "plots/abstractmultiplegraphsareaswidget.h"
#include "global_structs.h"
#include <QFuture>
#include <QFutureWatcher>

namespace Ui {
class StatisticGraphsArea;
}

class BoxPlot;

/*! Хранит на себе статистические графики */
class StatisticGraphsArea : public AbstractMultipleGraphsAreasWidget
{
    Q_OBJECT

    /*! Коробчатая диаграмма */
    BoxPlot *mBoxGraph {nullptr};

    /*! Два чередующихся режима приближения оси Y для BoxPlot
        1) Первый режим приближает относительно самой
        большой дистанции между меньшим и большим
        значение коробки.
        2) Второй режим приближает относительно самой
        большой дистанции между нижним и верхним квартилем.
    */
    bool mYZoomModeBoxGraph {true};

    /*! Кнопка для обновления текущего диапазона данных */
    AnimatedToolButton *mUpdateRangeToolButton {nullptr};

    /*! Результат асинхронных вычислений */
    QFuture<QCPStatisticalBoxDataContainer> mFuture;

    /*! Контроль прогресса асинхронных вычислений */
    QFutureWatcher<QCPStatisticalBoxDataContainer> mFutureWatcher;

    /*! Запрошенные данные у  */
    std::vector<ComplexValue> mRequestedData;

public:
    explicit StatisticGraphsArea(QWidget *parent = nullptr);
    ~StatisticGraphsArea();

private:
    /*! Отправляет запрос на обновление коробчаой диаграммы
        на указанном диапазоне в другой поток.
        xRange - диапазон, для которого будут рассчитываться коробки;
        countBox - кол-во рассчитываемых коробок.
        ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ:
        true - запрос успешно отправлен;
        false - запрос не отправлен.
    */
    bool updateRequestBoxGraphData(const QCPRange &xRange, int8_t countBox);

    /*################################################################################
                                Обновление диапазона оси Y
    ################################################################################*/
    /*! Обновление диапазона оси Y в соответствии с указанной функцией
        func - указатель на функцию, которая ищет максимальное и
        минимальное значение диапазона для отображения графика
    */
    bool updateYRange(void(*func)(QCPStatisticalBoxDataContainer *, double &, double &));

    /*! Обновление диапазона оси Y в соответствии с текущим режимом */
    bool updateCurrentYRange();

    /*! Настройка кнопки для обновления текущего диапазона данных */
    void setupUpdateCurrentRangeData();

private slots:
    /*! Обновление коробчатой диаграммы на текущем диапазоне
        true - данные успешно обновлены;
        false - данные не обновлены
    */
    bool updateBoxGraphOnCurrentRange();

public slots:
    /*! Ответ запроса на расчёт коробок */
    void responseUpdateRequestBoxGraph();

    void updateBoxGraph(const QCPStatisticalBoxDataContainer &data);

signals:
    void boxesIsReady();

private:
    Ui::StatisticGraphsArea *ui;

    // AbstractGraphAreaWidget interface
public:
    void installController(MonitorController *controller) override;
    void retranslate() override;
    void scaleFont(float scaleFactor) override;

protected slots:
    void replotDisplayedGraph() override;

    /*! Смена режима приближения по оси Y */
    void nextYRange() override;

    /*! Смена приближения по оси X */
    bool nextXRange() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // STATISTICGRAPHSAREA_H
