#ifndef MARKITEM_H
#define MARKITEM_H

#include "plots/qcustomplot.h"
#include <memory>
#include "global_define.h"

/*! Ориентация расположения метки */
enum LabelOrientation{
    moVerticalTop,
    moVerticalBottom
};

class Label;


class MarkItem : public QCPAbstractItem
{
    /*! Элемент текста */
    QCPItemText *mTextItem = nullptr;

    /*! Линия через область графика */
    QCPItemLine *mLineThroughGraph = nullptr;

    /*! Ориентация расположения метки */
    LabelOrientation mOrientation;

    /*! Текущая позиция метки относительно ориентации */
    double mCurrentPosition = 0;

public:
    uint32_t mIntervalPos = 0;

    double maxIntervalValue = -1;
    double averageIntervalValue = 0;

    MarkItem(QCustomPlot *parentPlot, const QString &text, const QFont &font, double level = 0, LabelOrientation orientation = LabelOrientation::moVerticalBottom);
    MarkItem(QCustomPlot *parentPlot, uint8_t num, QColor color, const QFont &font, const double position);
    MarkItem(QCustomPlot *parentPlot, const QString &text, const double position, const QFont &font);
    ~MarkItem();

    /*! Добавление пунктирной линии */
    void addLine();

    /*! Установка текста */
    void setText(const QString &text);

    /*! Отображение метки */
    void setVisible(bool enable);

    /*! Установка расположения метки */
    void setOrientation(LabelOrientation orientation);

    /*! Устанавливаем позицию метки */
    void setPositionMark(double position);

    /*! Отступ внутрь графика */
    void setMarkItemMargin(double level = 0);

    /*! Текущая позиция метки относительно ориентации */
    double getMarkPosition() const;

private:
    /*! Обновляет ориентацию метки */
    void updateMarkOrientation();

public:
    double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const override;
    void replotLine();
    void deleteLine();

protected:
    /*! Составной элемент из других, отрисовывать нечего */
    void draw(QCPPainter *painter) override;
};

#endif // MARKITEM_H
