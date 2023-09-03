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


uint32_t pointStart;
uint32_t pointStop;

bool AbstractCustomPlot::event(QEvent *event)
{
    // Узнаём тип события
    const auto typeOfEvent = event->type();
    //qDebug() << typeOfEvent ;
    static bool isLabelDrag = false;
    if ((mGraphType == RecordedGraph) && ((isLabelCreating == true) || (isIntervalCreating == true)))
    {
        if((typeOfEvent == QEvent::MouseButtonPress) || (typeOfEvent == QEvent::MouseButtonRelease) || (typeOfEvent == QEvent::MouseMove) )
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (typeOfEvent == QEvent::MouseButtonPress)
            {
                labelMoved = true;
                isLabelDrag = false;
                pointStart = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
                //qDebug() << "QEvent::MouseButtonPress: " << pointStart;
            }
            else if ((typeOfEvent == QEvent::MouseMove) && (labelMoved == true))
            {
                isLabelDrag = true;
                pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
                int32_t deltaX = pointStop - pointStart;
                if (abs(deltaX) > 2)
                {                    
                    if (isLabelCreating)
                    {
                        pointStart = pointStop;
                        mCoordLabelX += deltaX;
                        mLabelItemsContainer.back()->replotLine();
                    }
                    else if (isIntervalCreating)
                    {
                        auto ttt = (mIntervalsContainer[mIntervalsCount-1]->mIntervalPos + deltaX);
                        if (mIntervalsCount % 2 == 0)
                        {
                            if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos))
                            {
                                return true;
                            }
                        }
                        if (mIntervalsCount == 3)
                        {

                            if (ttt <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos))
                            {
                                return true;
                            }
                        }
//                        if (mIntervalsCount == 4)
//                        {
//                            if (mIntervalsContainer[2]->mIntervalPos < mIntervalsContainer[0]->mIntervalPos) //если третья точка была левее первой
//                            {
//                                if (ttt > (mIntervalsContainer[0]->mIntervalPos))
//                                {
//                                    return true;
//                                }
//                            }
//                        }
                        pointStart = pointStop;
                        //mIntervalPos += deltaX;
                        mIntervalsContainer[mIntervalsCount-1]->mIntervalPos += deltaX;
                        mIntervalsContainer[mIntervalsCount-1]->replotLine();
                    }
                }
                //qDebug() << "Delta: " << deltaX;
            }
            else if (typeOfEvent == QEvent::MouseButtonRelease)
            {
                pointStop = xAxis->pixelToCoord(mouseEvent->pos().x())*1000;
                labelMoved = false;
                if ((pointStart == pointStop) && (isLabelDrag == false))
                {
                    if (isLabelCreating)
                    {
                        mCoordLabelX = pointStart;
                        mLabelItemsContainer.back()->replotLine();
                    }
                    else if (isIntervalCreating)
                    {
                        if (mIntervalsCount % 2 == 0)
                        {
                            if (pointStart <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos))
                            {
                                return true;
                            }
                        }
                        if (mIntervalsCount == 3)
                        {
                            if (pointStart < (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos))
                            {
                                return true;
                            }
                        }
                        //mIntervalPos = pointStart;
                        mIntervalsContainer[mIntervalsCount-1]->mIntervalPos = pointStart;
                        mIntervalsContainer[mIntervalsCount-1]->replotLine();
                    }
                }
                isLabelDrag = false;
                //qDebug() << "QEvent::MouseButtonRelease: "  << pointStop << " Delta: " << deltaX;
            }
        }


        else if((typeOfEvent == QEvent::TouchBegin) || (typeOfEvent == QEvent::TouchEnd) || (typeOfEvent == QEvent::TouchUpdate) || (typeOfEvent == QEvent::TouchCancel) )
        {
            //доделать обработку 4 события
            QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
            if (typeOfEvent == QEvent::TouchBegin)
            {
                labelMoved = true;
                isLabelDrag = false;
                pointStart = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
                //qDebug() << "QEvent::MouseButtonPress: " << pointStart;
            }
            else if ((typeOfEvent == QEvent::TouchUpdate) && (labelMoved == true))
            {
                isLabelDrag = true;
                pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
                int32_t deltaX = pointStop - pointStart;
                if (abs(deltaX) > 2)
                {
                    pointStart = pointStop;
                    if (isLabelCreating)
                    {
                        mCoordLabelX += deltaX;
                        mLabelItemsContainer.back()->replotLine();
                    }
                    else if (isIntervalCreating)
                    {
                        if (mIntervalsCount % 2 == 0)
                        {
                            if ((mIntervalsContainer[mIntervalsCount-1]->mIntervalPos + deltaX) <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos))
                            {
                                return true;
                            }
                        }
                        //mIntervalPos += deltaX;
                        mIntervalsContainer[mIntervalsCount-1]->mIntervalPos += deltaX;
                        mIntervalsContainer[mIntervalsCount-1]->replotLine();

                    }
                }
                //qDebug() << "Delta: " << deltaX;
            }
            else if (typeOfEvent == QEvent::TouchEnd)
            {
                pointStop = xAxis->pixelToCoord(touchEvent->touchPoints().last().pos().x())*1000;
                labelMoved = false;
                if ((pointStart == pointStop) && (isLabelDrag == false))
                {
                    if (isLabelCreating)
                    {
                        mCoordLabelX = pointStart;
                        mLabelItemsContainer.back()->replotLine();
                    }
                    else if (isIntervalCreating)
                    {
                        if (mIntervalsCount % 2 == 0)
                        {
                            if (pointStart <= (mIntervalsContainer[mIntervalsCount-2]->mIntervalPos))
                            {
                                return true;
                            }
                        }
                        //mIntervalPos = pointStart;
                        mIntervalsContainer[mIntervalsCount-1]->mIntervalPos = pointStart;
                        mIntervalsContainer[mIntervalsCount-1]->replotLine();
                    }
                }
                isLabelDrag = false;
                //qDebug() << "QEvent::MouseButtonRelease: "  << pointStop << " Delta: " << deltaX;
            }

        }
    }

    // Если событие - взаимодействие с сенсором
    if(typeOfEvent == QEvent::TouchBegin ||
       typeOfEvent == QEvent::TouchEnd ||
       typeOfEvent == QEvent::TouchUpdate ||
       typeOfEvent == QEvent::TouchCancel)
    {
        // Кастуем
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

        // Получаем список точек касания
        auto& touchPoints = touchEvent->touchPoints();

        // Если нажали одним пальцем после нажатия двумя пальцами
        if(touchPoints.count() == 1 && mDoublePointsTouch) {
            // Отправляем событие для отпускания тача
            const auto &point = touchPoints.first();
            auto ev = QMouseEvent(QEvent::MouseButtonRelease, point.lastPos(),
                                  point.lastScenePos(), point.lastScreenPos(),
                                  Qt::MouseButton::LeftButton, Qt::MouseButtons(),
                                  Qt::KeyboardModifiers(Qt::KeyboardModifier::NoModifier),
                                  Qt::MouseEventSource::MouseEventSynthesizedBySystem);
            mouseReleaseEvent(&ev);

            // Обнуляем флаг нажатия двумя пальцами
            mDoublePointsTouch = false;

            // Отдаём событие на дальнейшую обработку
            event->ignore();

            // Событие было распознано
            return true;
        }

        // Если нажатие двумя пальцами
        if (touchPoints.count() == 2)
        {
            // Устанавливаем флаг нажатия двумя пальцами
            mDoublePointsTouch = true;

            // Сохраняем позиции точек
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();

            // Отношение текущей линии к предыдущей
            double scaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length() /
                    QLineF(touchPoint0.lastPos(), touchPoint1.lastPos()).length();

            // Центр прямоугольника (точка скейлинга)
            QPointF scalingPointF((touchPoint0.pos().x() + touchPoint1.pos().x())/2,
                                  (touchPoint0.pos().y() + touchPoint1.pos().y())/2);

            // Если было отдаление, делаем скейл отрицательным
            scaleFactor *= scaleFactor < 1 ? -1 : 1;

            // Если не отпустили пальцы, отправляем событие колеса мышки
            if(!touchEvent->touchPointStates().testFlag(Qt::TouchPointReleased)){
                QWheelEvent ev(scalingPointF, scalingPointF, QPoint(0, 0), QPoint(0, scaleFactor * scaleSensitivity),
                               Qt::MouseButton::NoButton, Qt::KeyboardModifier::NoModifier, Qt::ScrollPhase::NoScrollPhase, false);
                wheelEvent(&ev);
            }

            // Событие не требует дальнейшей обработки
            event->setAccepted(true);

            // Событие было распознано
            return true;
        }
    }

    // Если не распознали или не хотим обрабатывать событие, то отдаём родителю
    return QCustomPlot::event(event);
}

QPair<double, double> AbstractCustomPlot::setXRange(double lower, double upper)
{
    xAxis->setRange(lower, upper);
    return qMakePair(xAxis->range().lower, xAxis->range().upper);
}

void AbstractCustomPlot::setYRange(double lower, double upper)
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

void AbstractCustomPlot::scaleFont(double scaleFactor)
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
    setNotAntialiasedElements(QCP::aeAll); // Отключаем сглаживание всех элементов
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

    const double lowerTime = this->xAxis->range().lower;
    const double upperTime = this->xAxis->range().upper;
    const double hideTime = mOptimizeLabelsKoeff * (upperTime - lowerTime);

    mOptimizedItems[0]->setVisible(true);

    double lastVisibleTime = mOptimizedItems[0]->getMarkPosition();
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
