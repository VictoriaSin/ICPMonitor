#ifndef AVERAGEPOINTERWIDGET_H
#define AVERAGEPOINTERWIDGET_H

#include <QWidget>
#include <QPen>

class AveragePointerWidget : public QWidget
{
    Q_OBJECT

    QTimer *mBlinkAlarmTimer {nullptr};

    double mUpperValue {1};
    double mLowerValue {0};

    double mCurrentValue {0};

    QPen mLinePen;

    const QColor mBaseColorTriangle {255, 225, 0};

    QColor mFlashColor;
    QColor mPrevColor;

    QPolygon mTrianglePoly;
    QColor mTriangleColor;

    QPen mTrianglePen;
    QBrush mTriangleBrush;

    bool mIsPointerVisible {false};

    bool mSelectBetweenTwoColors {true};

public:
    explicit AveragePointerWidget(QWidget *parent = nullptr);

    void setRange(double lower, double upper);
    void setLowerValue(double lower);
    void setUpperValue(double upper);
    void setValue(double value);

    /*! Устанавливает цвет указателя (треугольника) на текущее значение на шкале
        color - новый цвет треугольника
     */
    void setColorTriangle(const QColor& color);

    /*! Запускает режим мигания
        color - цвет мигания
     */
    void startBlinkingMode(const QColor& color);

    /*! Останавливает режим мигания */
    void stopBlinkingMode();

    void setPointerVisible(bool visible);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void switchColor();
};

#endif // AVERAGEPOINTERWIDGET_H
