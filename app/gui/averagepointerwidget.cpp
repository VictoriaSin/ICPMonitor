#include "averagepointerwidget.h"

#include <QPainter>
#include <QtDebug>
#include <QTimer>

AveragePointerWidget::AveragePointerWidget(QWidget *parent) :
    QWidget{parent},
    mBlinkAlarmTimer(new QTimer(this))
{
    mBlinkAlarmTimer->setInterval(350);

    mLinePen = QPen{QBrush(QColor(240, 240, 240)), 4};
    mTriangleColor = mBaseColorTriangle;

    mTrianglePen = {mTriangleColor, 1};
    mTriangleBrush = {mTriangleColor};

    mTrianglePoly.append({0,0});
    mTrianglePoly.append({0,0});
    mTrianglePoly.append({0,0});

    connect(mBlinkAlarmTimer, &QTimer::timeout, this, &AveragePointerWidget::switchColor);
}

void AveragePointerWidget::setRange(float lower, float upper)
{
    mLowerValue = lower;
    mUpperValue = upper;

    if(qFuzzyCompare(mLowerValue, mUpperValue)){
        mLowerValue -= 1;
        mUpperValue += 1;
    }

    update();
}

void AveragePointerWidget::setLowerValue(float lower)
{
    setRange(lower, mUpperValue);
}

void AveragePointerWidget::setUpperValue(float upper)
{
    setRange(mLowerValue, upper);
}

void AveragePointerWidget::setValue(float value)
{
    mCurrentValue = value;

    update();
}

void AveragePointerWidget::setColorTriangle(const QColor &color)
{
    mTriangleColor = color;
    mTrianglePen.setColor(mTriangleColor);
    mTriangleBrush.setColor(mTriangleColor);
    update();
}

void AveragePointerWidget::startBlinkingMode(const QColor &color)
{
    mFlashColor = color;
    mBlinkAlarmTimer->start();
}

void AveragePointerWidget::stopBlinkingMode()
{
    setColorTriangle(mBaseColorTriangle);
    mBlinkAlarmTimer->stop();
}

void AveragePointerWidget::setPointerVisible(bool visible)
{
    mIsPointerVisible = visible;

    update();
}

void AveragePointerWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setFont(font());

    const auto geom = rect();
    const int height9 = geom.height() / 9;

    const int vertLineX = geom.width() / 2.5; ///< Позиция X вертикальной линии
    const int labelX = vertLineX + 10; ///< Позиция X подписей
    const int horLineStartX = geom.width() / 5; ///< Позиция X начала горизонтальных линий

    const int upPosY = height9;  ///< Начало (верх) вертикальной линии по оси Y
    const int loPosY = geom.height() - height9; ///< Конец (низ) вертикальной линии по оси Y

    const int triangleHeight = geom.width() / 3; ///< Высота (h) равностороннего треугольника-указателя

    const auto correctLabelYPos = painter.fontInfo().pixelSize() / 2.5; ///< Коррекция позиции Y подписей для того, чтобы они располагались посередине горизонтальных линий

    // draw lines
    painter.setPen(mLinePen);
    // draw vert line
    painter.drawLine(vertLineX, 0, vertLineX, geom.height());
    // draw hor lines
    painter.drawLine(horLineStartX, upPosY, vertLineX, upPosY);
    painter.drawLine(horLineStartX, loPosY, vertLineX, loPosY);

    // draw labels
    painter.drawText(labelX, upPosY + correctLabelYPos, QString::number(mUpperValue));
    painter.drawText(labelX, loPosY + correctLabelYPos, QString::number(mLowerValue));

    if(mIsPointerVisible){
        // draw triangle
        int pointerPosY = loPosY; ///< Позиция указателя по Y
        const int pointerPosX = vertLineX - mLinePen.width(); ///< Позиция указателя по X (с коррекций на ширину вертикальной линии)

        if(mCurrentValue <= mUpperValue && mCurrentValue >= mLowerValue){
            pointerPosY = ((loPosY - upPosY) / (mLowerValue - mUpperValue)) * (mCurrentValue - mUpperValue) + upPosY; // По уравнению прямой получаем значение
        }
        else if(mCurrentValue > mUpperValue){
            pointerPosY = upPosY;
        }
        else if(mCurrentValue < mLowerValue){
            pointerPosY = loPosY;
        }

        const int triangleLeftX = pointerPosX - triangleHeight; ///< Позиция X левой (вертикальной) стороны треугольника
        const float dYTr = triangleHeight * 0.577; ///< Половина длины стороны треугольника

        mTrianglePoly[0].setX(triangleLeftX);
        mTrianglePoly[0].setY(int(pointerPosY-dYTr));

        mTrianglePoly[1].setX(pointerPosX);
        mTrianglePoly[1].setY(pointerPosY);

        mTrianglePoly[2].setX(triangleLeftX);
        mTrianglePoly[2].setY(int(pointerPosY+dYTr));

        painter.setPen(mTrianglePen);
        painter.setBrush(mTriangleBrush);
        painter.drawPolygon(mTrianglePoly);
    }
}

void AveragePointerWidget::switchColor()
{
    setColorTriangle(mSelectBetweenTwoColors ? mFlashColor : mBaseColorTriangle);
    mSelectBetweenTwoColors = !mSelectBetweenTwoColors;
}
