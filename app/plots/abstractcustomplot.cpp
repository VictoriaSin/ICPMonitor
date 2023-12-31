#include "abstractcustomplot.h"
#include "plots/markitem.h"
#include <QPaintEngine>

#include "plots/labelmarkitem.h"

AbstractCustomPlot::AbstractCustomPlot(GraphType type, QWidget *parent)
    : QCustomPlot(parent), mGraphType(type)
{
    // Настройка атрибутов для отрисовки виджета
    setAttribute(Qt::WA_AcceptTouchEvents); // Разрешаем взаимодействием с тачем
    setAttribute(Qt::WA_OpaquePaintEvent); // Для быстрой отрисовки виджетов (не стирает предыдущую отрисовку, для систем без двойной буферизации)

    setDirectPainting(true); // Режим прямого рисования (Custom)
    setInteraction(QCP::iRangeDrag, true); // Можем передвигать график
    setInteraction(QCP::iRangeZoom, true); // Можем приближать график

    // Настройка сглаживания декартовых координат
    settingAnalisingElements();

    // Настройка отрисовки декартовых координат
    settingUpCartesianCoordinates();



}

#ifdef PC_BUILD
bool AbstractCustomPlot::editLabel(QMouseEvent *mouseEvent)
{
    const auto typeOfEvent = mouseEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) )
    {
        if (typeOfEvent == QEvent::MouseButtonPress)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::MouseMove) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                pointStart = pointStop;
                mLabelItemsContainer.back()->mIntervalPos += deltaX;
                //mCoordLabelX += deltaX;
                mLabelItemsContainer.back()->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::MouseButtonRelease)
        {
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                //mCoordLabelX = pointStart;
                mLabelItemsContainer.back()->mIntervalPos = pointStart;
                mLabelItemsContainer.back()->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}
bool AbstractCustomPlot::editInterval(QMouseEvent *mouseEvent)
{
    const auto typeOfEvent = mouseEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) )
    {
        if (typeOfEvent == QEvent::MouseButtonPress)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::MouseMove) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                auto ttt = (mIntervalsContainer[mIntervalsCount-1]->mIntervalPos + deltaX);
                if (mIntervalsCount % 2 == 0)
                {
                    if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                    if (mIntervalsCount == 3)
                    {
                        if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                    }
                pointStart = pointStop;
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos += deltaX;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::MouseButtonRelease)
        {
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                if (mIntervalsCount % 2 == 0)
                {
                    if (pointStart <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                if (mIntervalsCount == 3)
                {
                    if (pointStart < (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos = pointStart;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}

bool AbstractCustomPlot::editFluidInterval(QMouseEvent *mouseEvent) // new func
{
    const auto typeOfEvent = mouseEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) )
    {
        if (typeOfEvent == QEvent::MouseButtonPress)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::MouseMove) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                if (mFluidMarksCounter == 2)
                {
                    auto ttt = (mFluidMarkContainer[mFluidMarksCounter-1]->mIntervalPos + deltaX);
                    if (ttt <= (mFluidMarkContainer[mFluidMarksCounter-2]->mIntervalPos)) { return true; }
                }

                pointStart = pointStop;
                mFluidMarkContainer[mFluidMarksCounter-1]->mIntervalPos += deltaX;
                mFluidMarkContainer[mFluidMarksCounter-1]->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::MouseButtonRelease)
        {
            pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                if (mFluidMarksCounter == 2)
                {
                    if (pointStart <= (mFluidMarkContainer[mFluidMarksCounter-2]->mIntervalPos)) { return true; }
                }
                mFluidMarkContainer[mFluidMarksCounter-1]->mIntervalPos = pointStart;
                mFluidMarkContainer[mFluidMarksCounter-1]->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}

bool AbstractCustomPlot::editAxisRange(QMouseEvent *mouseEvent, float minX, float maxX, float maxY)
{
    const auto typeOfEvent = mouseEvent->type();
    static bool isAxisMoving = false;
    static double pointStartX, pointStartY;
    static double pointStopX, pointStopY;

    if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) || (typeOfEvent == QEvent::Wheel))
    {
        if (typeOfEvent == QEvent::MouseButtonPress)
        {
            isAxisMoving = true;
            pointStartX = xAxis->pixelToCoord(mouseEvent->pos().x());
            pointStartY = yAxis->pixelToCoord(mouseEvent->pos().y());
            return true;
        }
        if ((typeOfEvent == QEvent::MouseMove) && (isAxisMoving))
        {
            pointStopX = xAxis->pixelToCoord(mouseEvent->pos().x());
            pointStopY = yAxis->pixelToCoord(mouseEvent->pos().y());
            double deltaX = pointStopX - pointStartX;
            double deltaY = pointStopY - pointStartY;

            if (!((xAxis->range().lower - deltaX < minX) || (xAxis->range().upper - deltaX > maxX)))
            {
                //qDebug() << "minX" << minX << "lowX" << xAxis->range().lower << "delta" << deltaX;
                xAxis->setRange(xAxis->range().lower - deltaX, xAxis->range().upper - deltaX);
            }
            if (!((yAxis->range().lower - deltaY < 0) || (yAxis->range().upper - deltaY > maxY)))
            {
                //qDebug() << "maxY" << maxY << "lowY" << yAxis->range().lower << "delta" << deltaY;
                yAxis->setRange(yAxis->range().lower - deltaY, yAxis->range().upper - deltaY);
            }

            pointStartX = pointStopX = xAxis->pixelToCoord(mouseEvent->pos().x());
            pointStartY = pointStopY = yAxis->pixelToCoord(mouseEvent->pos().y());
            return true;
        }
        if (typeOfEvent == QEvent::MouseButtonRelease)
        {
            isAxisMoving = false;
            return true;
        }

        if (typeOfEvent == QEvent::Wheel)
        {
            #define DEF_ZOOM_IN_X  (double)0.9
            #define DEF_ZOOM_OUT_X (double)(1.0/DEF_ZOOM_IN_X)
            QWheelEvent *eventWheel = (QWheelEvent*)mouseEvent;
            pointStartX = xAxis->pixelToCoord(mouseEvent->pos().x());

            if (eventWheel->delta() < 0)
            {
              double startPlot = minX;
              double endPlot = xAxis->range().upper / DEF_ZOOM_IN_X;
              bool customPlot = false;
              if (xAxis->range().lower - (pointStartX - (pointStartX * DEF_ZOOM_IN_X)) < minX ) // < minX
              {
                customPlot = true;
              }

              if (endPlot > maxX)
              {
                if (!customPlot) startPlot = xAxis->range().lower * DEF_ZOOM_IN_X;
                customPlot = true;
                endPlot = maxX;
              }

              if (customPlot)
              {
                xAxis->setRange(startPlot, endPlot);
                return true;
              }
              xAxis->scaleRange(DEF_ZOOM_OUT_X, pointStartX);
              return true;
            }

            xAxis->scaleRange(DEF_ZOOM_IN_X, pointStartX); // увеличение
            return true;
        }
    }
    return false;
}
#else
// Для тача
bool AbstractCustomPlot::editLabel(QTouchEvent *touchEvent)
{
    const auto typeOfEvent = touchEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate) )
    {
        if (typeOfEvent == QEvent::TouchBegin)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::TouchUpdate) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                pointStart = pointStop;
                //mCoordLabelX += deltaX;
                mLabelItemsContainer.back()->mIntervalPos += deltaX;
                mLabelItemsContainer.back()->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::TouchEnd)
        {
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                //mCoordLabelX = pointStart;
                mLabelItemsContainer.back()->mIntervalPos = pointStart;
                mLabelItemsContainer.back()->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}

bool AbstractCustomPlot::editInterval(QTouchEvent *touchEvent)
{
    const auto typeOfEvent = touchEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate) )
    {
        if (typeOfEvent == QEvent::TouchBegin)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::TouchUpdate) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                auto ttt = (mIntervalsContainer[mIntervalsCount-1]->mIntervalPos + deltaX);
                if (mIntervalsCount % 2 == 0)
                {
                    if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                    if (mIntervalsCount == 3)
                    {
                        if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                    }
                pointStart = pointStop;
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos += deltaX;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::TouchEnd)
        {
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                if (mIntervalsCount % 2 == 0)
                {
                    if (pointStart <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                else
                if (mIntervalsCount == 3)
                {
                    if (pointStart < (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos)) { return true; }
                }
                mIntervalsContainer[mIntervalsCount-1]->mIntervalPos = pointStart;
                mIntervalsContainer[mIntervalsCount-1]->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}

bool AbstractCustomPlot::editFluidInterval(QTouchEvent *touchEvent) // new func
{
    const auto typeOfEvent = touchEvent->type();
    static bool isLabelDrag = false;
    if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate) )
    {
        if (typeOfEvent == QEvent::TouchBegin)
        {
            labelMoved = true;
            isLabelDrag = false;
            pointStart = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            return true;
        }
        if ((typeOfEvent == QEvent::TouchUpdate) && (labelMoved == true))
        {
            isLabelDrag = true;
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            int32_t deltaX = pointStop - pointStart;
            if (abs(deltaX) > 2)
            {
                if (mFluidMarksCounter == 2)
                {
                    auto ttt = (mFluidMarkContainer[mFluidMarksCounter-1]->mIntervalPos + deltaX);
                    if (ttt <= (mFluidMarkContainer[mFluidMarksCounter-2]->mIntervalPos)) { return true; }
                }

                pointStart = pointStop;
                mFluidMarkContainer[mFluidMarksCounter-1]->mIntervalPos += deltaX;
                mFluidMarkContainer[mFluidMarksCounter-1]->replotLine();
            }
            return true;
        }
        if (typeOfEvent == QEvent::TouchEnd)
        {
            pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
            labelMoved = false;
            if ((pointStart == pointStop) && (isLabelDrag == false))
            {
                if (mFluidMarksCounter == 2)
                {
                    if (pointStart <= (mFluidMarkContainer[mFluidMarksCounter-2]->mIntervalPos)) { return true; }
                }
                mFluidMarkContainer[mFluidMarksCounter-1]->mIntervalPos = pointStart;
                mFluidMarkContainer[mFluidMarksCounter-1]->replotLine();
            }
            isLabelDrag = false;
            return true;
        }
    }
    return false;
}


bool AbstractCustomPlot::editAxisRange(QTouchEvent *touchEvent, float minX, float maxX, float maxY)
{
    const auto typeOfEvent = touchEvent->type();
    static bool isAxisMoving = false;
    static float pointStartX, pointStartY;
    static float pointStopX, pointStopY;

    if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate))// || (typeOfEvent == QEvent::Wheel))
    {
        if (typeOfEvent == QEvent::TouchBegin)
        {
            isAxisMoving = true;
            pointStartX = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x());
            pointStartY = yAxis->pixelToCoord(touchEvent->touchPoints().last().pos().y());
            return true;
        }
        if ((typeOfEvent == QEvent::TouchUpdate) && (isAxisMoving))
        {
            pointStopX = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x());
            pointStopY = yAxis->pixelToCoord(touchEvent->touchPoints().last().pos().y());
            //qDebug() << "pointStopY" << pointStopY << "pointStartY" << pointStartY;
            double deltaX = pointStopX - pointStartX;
            double deltaY = pointStopY - pointStartY;

            if (!((xAxis->range().lower - deltaX < minX) || (xAxis->range().upper - deltaX > maxX)))
            {
                xAxis->setRange(xAxis->range().lower - deltaX, xAxis->range().upper - deltaX);
            }
            if (!((yAxis->range().lower - deltaY < 0) || (yAxis->range().upper - deltaY > maxY)))
            {
                //qDebug() << "maxY" << maxY << "lowY" << yAxis->range().lower << "delta" << deltaY;
                yAxis->setRange(yAxis->range().lower - deltaY, yAxis->range().upper - deltaY);
            }

            pointStartX = pointStopX = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x());
            pointStartY = pointStopY = yAxis->pixelToCoord(touchEvent->touchPoints().last().pos().y());
            return true;
        }
        if (typeOfEvent == QEvent::TouchEnd)
        {
            isAxisMoving = false;
            return true;
        }

        /*if (typeOfEvent == QEvent::Wheel)
        {
            #define DEF_ZOOM_IN_X  (float)0.8
            #define DEF_ZOOM_OUT_X (float)(1.0/DEF_ZOOM_IN_X)
            QWheelEvent *eventWheel = (QWheelEvent*)mouseEvent;
            pointStartX = xAxis->pixelToCoord(mouseEvent->pos().x());

            if (eventWheel->delta() < 0)
            {
              //qDebug("Zoom-");
              //qDebug() << "pointStartX:" << pointStartX << " (pointStartX *  DEF_ZOOM_IN_X)" << (pointStartX *  DEF_ZOOM_IN_X) << " xAxis->range().lower" << xAxis->range().lower;
              float startPlot;
              float endPlot;
              bool customPlot = false;
              if ((pointStartX - (pointStartX * DEF_ZOOM_IN_X)) > xAxis->range().lower) // < 0
              {
                customPlot = true;
                startPlot = 0;
              }

              if ((xAxis->range().upper + (xAxis->range().upper *  DEF_ZOOM_IN_X)) > mRecordedMaxXRange) // > MAX
              {
                customPlot = true;
                startPlot = xAxis->range().lower * DEF_ZOOM_IN_X;
                endPlot = maxX;
              }
              else
              {
                endPlot = xAxis->range().size() / DEF_ZOOM_IN_X;
              }

              if (customPlot)
              {
                xAxis->setRange(startPlot, endPlot);
                return true;
              }
              xAxis->scaleRange(DEF_ZOOM_OUT_X, pointStartX);
              return true;
            }
            else
            {
              //qDebug("Zoom+");
              xAxis->scaleRange(DEF_ZOOM_IN_X, pointStartX);
            }
            return true;
        }*/
    }
    return false;
}

#endif

bool AbstractCustomPlot::event(QEvent *event)
{
    if (mGraphType == RecordedGraph)        
    {
#ifdef PC_BUILD
        const auto typeOfEvent = event->type();
        if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) || (typeOfEvent == QEvent::Wheel))
        {
            if (isLabelCreating == true)
            {
                if (editLabel((QMouseEvent*)event)) return true;
            }
            else if (isIntervalCreating == true)
            {
                if (editInterval((QMouseEvent*)event)) return true;
            }
            else if (isFluidIntervalCreating == true)
            {
                if (editFluidInterval((QMouseEvent*)event)) return true; // new func
            }
//            else
//            {
//                if (editAxisRange((QMouseEvent*)event, 0, mRecordedMaxXRange, mCurrentMaxYRange)) return true;
//            }
        }
#else

        if (isLabelCreating == true)
        {
            if (editLabel((QTouchEvent*)event)) return true;
        }
        else if (isIntervalCreating == true)
        {
            if (editInterval((QTouchEvent*)event)) return true;
        }
        else if (isFluidIntervalCreating == true)
        {
            if (editFluidInterval((QTouchEvent*)event)) return true; // new func
        }
        else
        {
            if (editAxisRange((QTouchEvent*)event, 0, mRecordedMaxXRange, mCurrentMaxYRange)) return true;
        }
#endif
    }
    else if (mGraphType == IntervalGraph)
    {
        const auto typeOfEvent = event->type();
#ifdef PC_BUILD

        if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) || (typeOfEvent == QEvent::Wheel))
        {
            if (mCurrentIntervalNum == 1)
            {
                if (editAxisRange((QMouseEvent*)event, mFirstIntervalMinMaxXRange.first, mFirstIntervalMinMaxXRange.second, mCurrentMaxYRange)) return true;
            }
            else if (mCurrentIntervalNum == 2)
            {
                if (editAxisRange((QMouseEvent*)event, mSecondIntervalMinMaxXRange.first, mSecondIntervalMinMaxXRange.second, mCurrentMaxYRange)) return true;
            }
            return true;
        }

#else
        if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate))// || (typeOfEvent == QEvent::Wheel))
        {
            if (mCurrentIntervalNum == 1)
            {
                if (editAxisRange((QTouchEvent*)event, mFirstIntervalMinMaxXRange.first, mFirstIntervalMinMaxXRange.second, mCurrentMaxYRange)) return true;
            }
            else if (mCurrentIntervalNum == 2)
            {
                if (editAxisRange((QTouchEvent*)event, mSecondIntervalMinMaxXRange.first, mSecondIntervalMinMaxXRange.second, mCurrentMaxYRange)) return true;
            }
            return true;
        }
#endif    
    }
    return QCustomPlot::event(event);
}

QPair<float, float> AbstractCustomPlot::setXRange(float lower, float upper)
{
    xAxis->setRange(lower, upper);
    return qMakePair(xAxis->range().lower, xAxis->range().upper);
}

void AbstractCustomPlot::setYRange(float lower, float upper)
{
    yAxis->setRange(lower, upper);
}

AbstractCustomPlot::GraphType AbstractCustomPlot::type() const
{
    return mGraphType;
}

void AbstractCustomPlot::setAdaptiveSamplingLabelItems(bool enabled)
{
    mAdaptiveSamplingLabelItems = enabled;
}

void AbstractCustomPlot::scaleFont(float scaleFactor)
{
    Q_UNUSED(scaleFactor);
}

void AbstractCustomPlot::resetGraph()
{
    // Если график существует
    if (auto gr = graph()) {
        gr->data().clear();
    }
}

void AbstractCustomPlot::settingUpCartesianCoordinates()
{
    // Настройка фона для декартовых координат
    setBackground(QColor("#0C2731"));

    // Настройка оси X (нижней)
    settingUpXAxis();

    // Настройка оси X (верхней)
    settingUpXAxis2();

    // Настройка оси Y (левой)
    settingUpYAxis();

    // Настройка оси Y (правой)
    settingUpYAxis2();
}

void AbstractCustomPlot::settingUpXAxis()
{
    // Подготавливаем кисть
    QBrush brush;
    brush.setColor(Qt::white);
    brush.setStyle(Qt::SolidPattern);

    // Подготавливаем ручку
    QPen pen;
    pen.setBrush(brush);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);

    // Подготавливаем шрифт
    QFont font;
    font.setBold(true);
    font.setFamily("Sans Serif");
    font.setPointSize(10);

    // Ручка рисования для оси
    xAxis->setBasePen(pen);

    // Ручка рисования для разделителей на уровне сетки
    xAxis->setTickPen(pen);

    // Длина тикеров
    xAxis->setTickLength(7, 7);

    // Подготавливаем ручку для рисования подразделителей
    pen.setWidth(1);

    // Ручка для рисования подразделителей между разделителями
    xAxis->setSubTickPen(pen);

    // Длина сабтикеров
    xAxis->setSubTickLength(3, 3);

    // Цвет подписей на разделителях на уровне сетки
    xAxis->setTickLabelColor(Qt::white);

    // Шрифт текста на осях
    xAxis->setTickLabelFont(font);

    // Подготавливаем шрифт для подписей к осям
    font.setFamily("Sans Serif");
    font.setBold(true);
    font.setItalic(true);
    font.setPointSize(10);

    // Цвет подписей у осей
    xAxis->setLabelColor(Qt::white);

    // Шрифт текста
    xAxis->setLabelFont(font);

    // Настройка деления осей
    xAxis->ticker()->setTickCount(6);

    // Настройка диапазона значений осей
    xAxis->setRange(0, 12);
}
void AbstractCustomPlot::settingUpXAxis2()
{
    // Подготавливаем кисть
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setBrush(brush);

    // Подготавливаем шрифт
    QFont font;
    font.setBold(true);
    font.setFamily("Sans Serif");
    font.setPointSize(12);

    // Ручка рисования
    xAxis2->setBasePen(pen);

    // Устанавливаем шрифт
    xAxis2->setLabelFont(font);

    // Устанавливаем цвет подписей
    xAxis2->setLabelColor(Qt::white);

    // Цвет подписей на разделителях на уровне сетки
    xAxis2->setTickLabelColor(Qt::white);
    // Шрифт текста на осях
    xAxis2->setTickLabelFont(font);

    // Отключаем тики и сабтики
    xAxis2->setTickLabels(false);
    xAxis2->setTicks(false);
    xAxis2->setSubTicks(false);
}
void AbstractCustomPlot::settingUpYAxis()
{
    // Подготавливаем кисть
    QBrush brush;
    brush.setColor(Qt::white);
    brush.setStyle(Qt::SolidPattern);

    // Подготавливаем ручку
    QPen pen;
    pen.setBrush(brush);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);

    // Подготавливаем шрифт
    QFont font;
    font.setBold(true);
    font.setFamily("Sans Serif");
    font.setPointSize(10);

    // Формат подписей делений оси
    yAxis->setNumberFormat("f");
    yAxis->setNumberPrecision(1);

    // Ручка рисования для оси
    yAxis->setBasePen(pen);

    // Ручка рисования для разделителей на уровне сетки
    yAxis->setTickPen(pen);

    // Длина тикеров
    yAxis->setTickLength(7, 7);

    // Подготавливаем ручку для рисования подразделителей
    pen.setWidth(1);

    // Ручка для рисования подразделителей между разделителями
    yAxis->setSubTickPen(pen);

    // Длина сабтикеров
    yAxis->setSubTickLength(3, 3);

    // Цвет подписей на разделителях на уровне сетки
    yAxis->setTickLabelColor(Qt::white);

    // Шрифт текста у тикеров
    yAxis->setTickLabelFont(font);

    // Подготавливаем шрифт для подписей к осям
    font.setFamily("Sans Serif");
    font.setBold(true);
    font.setItalic(true);
    font.setPointSize(10);

    // Цвет подписей у осей
    yAxis->setLabelColor(Qt::white);

    // Шрифт текста
    yAxis->setLabelFont(font);

    // Настройка деления осей
    yAxis->ticker()->setTickCount(3);
    //yAxis->setNumberFormat("f");
    //yAxis->setNumberPrecision(2);

    // Настройка диапазона значений осей
    yAxis->setRange(10, 35);
}
void AbstractCustomPlot::settingUpYAxis2()
{
    // Подготавливаем кисть
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setBrush(brush);

    // Подготавливаем шрифт
    QFont font;
    font.setBold(true);
    font.setFamily("Sans Serif");
    font.setItalic(true);
    font.setPointSize(12);

    // Ручка рисования
    xAxis2->setBasePen(pen);

    // Устанавливаем шрифт
    yAxis2->setLabelFont(font);

    // Устанавливаем цвет подписей
    yAxis2->setLabelColor(Qt::white);

    // Отключаем тики
    yAxis2->setTickLabels(false);
    yAxis2->setTicks(false);
}
void AbstractCustomPlot::settingAnalisingElements()
{
    setNoAntialiasingOnDrag(false); // Включаем сглаживание при перетаскивании
    setNotAntialiasedElements(QCP::aeNone); // Отключаем сглаживание всех элементов
    setAntialiasedElement(QCP::aePlottables, true); // Включаем сглаживание для графиков

    // При взаимодействии пользователя с тачем включаем сглаживание
    selectionRect()->setAntialiased(true);
}
void AbstractCustomPlot::labelItemsOptimization()
{
    // Если оптимизация не включена
    if (!mAdaptiveSamplingLabelItems) {
        return;
    }

    int count = optimizationLabelItemCount();
    if (count == 0){
        return;
    }

    const float lowerTime = this->xAxis->range().lower;
    const float upperTime = this->xAxis->range().upper;
    const float hideTime = mOptimizeLabelsKoeff * (upperTime - lowerTime);

    mOptimizedItems[0]->setVisible(true);

    float lastVisibleTime = mOptimizedItems[0]->getMarkPosition();
    for (int i = 1; i < count; ++i) {
        MarkItem* const label= mOptimizedItems[i];
        if (label->getMarkPosition() < (lastVisibleTime + hideTime)) {
            label->setVisible(false);
        } else {
            lastVisibleTime = label->getMarkPosition();
            label->setVisible(true);
        }
    }
}
bool AbstractCustomPlot::setOptimizationLabelItems(const QList<MarkItem *> items)
{
    for (auto &item : qAsConst(items)) {
        // Если элемент не добавился
        if (!addOptimizationLabelItem(item)) {
            clearOtimizationLabelItems();
            return false;
        }
    }

    return true;
}
bool AbstractCustomPlot::addOptimizationLabelItem(MarkItem *item)
{
    // Если нет элемента на графике, не добавляем
    if (!hasItem(item)) {
        return false;
    }

    // Добавляем
    mOptimizedItems.append(item);

    // Оптимизируем отображение
    labelItemsOptimization();

    return true;
}
MarkItem *AbstractCustomPlot::optimizationLabelItem(int index) const
{
    if (index >= 0 && index < mOptimizedItems.size())
    {
        return mOptimizedItems.at(index);
    } else {
        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
        return nullptr;
    }
}
MarkItem *AbstractCustomPlot::optimizationLabelItem() const
{
    if (!mOptimizedItems.isEmpty())
    {
        return mOptimizedItems.last();
    } else {
        return nullptr;
    }
}
bool AbstractCustomPlot::removeOptimizationLabelItem(MarkItem *item)
{
    // Если элемента нет в списке
    if (!hasOptimizationLabelItem(item)) {
        return false;
    }

    mOptimizedItems.removeOne(item);
    return true;
}
bool AbstractCustomPlot::removeOptimizationLabelItem(int index)
{
    if (index >= 0 && index < mOptimizedItems.size()) {
        mOptimizedItems.removeAt(index);
        return true;
    } else {
        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
        return false;
    }
}
void AbstractCustomPlot::clearOtimizationLabelItems()
{
    mOptimizedItems.clear();
}
bool AbstractCustomPlot::hasOptimizationLabelItem(MarkItem *item) const
{
    return mOptimizedItems.contains(item);
}
int AbstractCustomPlot::optimizationLabelItemCount() const
{
    return mOptimizedItems.size();
}


