#ifndef ABSTRACTMULTIPLEGRAPHSAREASWIDGET_H
#define ABSTRACTMULTIPLEGRAPHSAREASWIDGET_H

#include "plots/abstractgraphareawidget.h"
#include "plots/abstractcustomplot.h"

class AnimatedToolButton;

/*! Абстрактный класс для отображения нескольких графиков
    с возможностью реализации нажатия кнопок для
    приближения/отдаления и передвижении графика.
    Для обновления графиков необходимо реализовать
    метод replotDisplayedGraph. Базовая частота
    обновления экрана задана 50 Гц. При необходимости
    можно добавить дополнительные кнопки в левый
    лэйаут при помощи соответствующего метода.
    При инициализации графиков необходимо добавить их
    в контейнер mGraphContainer, оставить видимым лишь один
    и указать в соответствии с индексацией контейнера текущий
    отображаемый график в переменную mCurrentGraphIndex и тип
    графика в mDisplayedGraph.
*/
class AbstractMultipleGraphsAreasWidget : public AbstractGraphAreaWidget
{
    Q_OBJECT

protected:
    /*! Контейнер с графиками */
    QVector<AbstractCustomPlot *> mGraphContainer;

    /*! Текущий отображаемый график */
    AbstractCustomPlot::GraphType mDisplayedGraph;

    /*! Индекс текущего графика */
    uint8_t mCurrentGraphIndex {0};

    /*! Кнопка для смены графика */
    AnimatedToolButton *mChangeGraphToolButton {nullptr};

public:
    explicit AbstractMultipleGraphsAreasWidget(QWidget *parent = nullptr);
    ~AbstractMultipleGraphsAreasWidget();

protected:
    /*! Обновление имени установленного графика */
    virtual void updateDisplayedGraphName() override;

private:
    /*! Настройка кнопки смены графика */
    void setupChangeGraphToolButton();

protected slots:


    // AbstractGraphAreaWidget interface
public:
    void retranslate() override;
    void scaleFont(float scaleFactor) override;

public slots:
    /*! Смена графика */
    virtual void changeGraph();
//    void changeButtonStatus();
};

#endif // ABSTRACTMULTIPLEGRAPHSAREASWIDGET_H
