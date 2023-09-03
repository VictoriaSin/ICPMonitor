#ifndef TRENDPLOT_H
#define TRENDPLOT_H

#include "abstractcustomplot.h"
#include "global_structs.h"

class MarkItem;
class LabelManager;
class TextEditDialog;

class TrendPlot : public AbstractCustomPlot
{
    Q_OBJECT

    /*! График с накапливаемыми средними значениями */
    QCPGraph *mAverageGraph {nullptr};

    /*! Бесконечная линия верхнего уровня тревоги */
    QCPItemStraightLine *mUpperAlarmLimit {nullptr};

    /*! Бесконечная линия нижнего уровня тревоги */
    QCPItemStraightLine *mLowerAlarmLimit {nullptr};

    /*! Менеджер меток */
    LabelManager *mLabelManager {nullptr};

    /*! Окно для просмотра и редактирования информации о метах */
    TextEditDialog *mTextEditDialog {nullptr};

    /*! Шаблон для заполнения информации о метке */
    const QString TemplateInfoAboutLabel = "%1: %2\n"
                                           "%3: %4\n"
                                           "%5: %6\n"
                                           "%7: %8\n"
                                           "%9:";

public:
    explicit TrendPlot(QWidget *parent = nullptr);
    ~TrendPlot();

    /*! Установка контейнера данных для отображения */
    void setDataContainerForGraph(QSharedPointer<QCPGraphDataContainer> data);

    /*! Установка менеджера меток */
    void setLabelManager(LabelManager *labelManager);

    /*! Данные графика */
    QSharedPointer<QCPGraphDataContainer> data() const;

    /*! Перевод */
    void retranslate();

public slots:
    /*! Установка линии верхнего уровня тревоги */
    void setUpperAlarmLevelLine(double upperAlarmLimit);

    /*! Установка линии нижнего уровня тревоги */
    void setLowerAlarmLevelLine(double lowerAlarmLimit);

    /*! Включение/Выключение верхнего уровня тревоги */
    void enableUpperAlarm(bool enable);

    /*! Включение/Выключение нижнего уровня тревоги */
    void enableLowerAlarm(bool enable);

protected slots:
    /*! Обработка нажатий по элементам графика */
    void itemClicked(QCPAbstractItem *item, QMouseEvent *event);

    // AbstractCustomPlot interface
public:
    void resetGraph() override;

    void scaleFont(double scaleFactor) override;
};

#endif // TRENDPLOT_H
