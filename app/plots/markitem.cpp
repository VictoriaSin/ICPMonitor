#include "markitem.h"


MarkItem::MarkItem(QCustomPlot *parentPlot, const QString &text, const QFont &font, double level, LabelOrientation orientation) :
    QCPAbstractItem(parentPlot),
    mTextItem(new QCPItemText(parentPlot)),
    mOrientation(orientation)
{
    // Установка слоя отрисовки
    setLayer("overlay");

    // Установка ориентации
    setOrientation(mOrientation);

    // Отступ внутрь графика
    setMarkItemMargin(level);

    // Настройка отрисовки элемента текста
    mTextItem->setText(text); // Установка текста
    mTextItem->setPen(QColor(Qt::black)); // Отрисовка прямоугольника
    mTextItem->setBrush(QColor(Qt::yellow)); // Отрисовка заднего фона прямоугольника
    mTextItem->setColor(QColor("#0c2731")); // Цвет текста
    mTextItem->setLayer("legend"); // На один слой ниже чем this
    mTextItem->setPadding(QMargins(3, 0, 3, 0)); // Отступы от рамки прямоугольника к тексту
    mTextItem->setFont(font); // Устанавливаем базовый шрифт
    mTextItem->setSelectable(false); // Устанавливаем запрет на выбор элемента кликом


}

MarkItem::MarkItem(QCustomPlot *parentPlot, uint8_t num, QColor color, const QFont &font, const double position) :
    QCPAbstractItem(parentPlot),
    mTextItem(new QCPItemText(parentPlot))
{
    qDebug() << "mark interval" << num;
    // Установка слоя отрисовки
    setLayer("overlay");

    // Установка ориентации
    setOrientation(moVerticalBottom);

    // Настройка отрисовки элемента текста
    if (num % 2 == 1)
    {
        mTextItem->setText("<" + QString::number(num/2 + 1)); // Установка текста
    }
    else
    {
        mTextItem->setText(QString::number(num/2 + 1)+ ">"); // Установка текста
    }

    mTextItem->setPen(QColor(Qt::black)); // Отрисовка прямоугольника
    mTextItem->setBrush(color); // Отрисовка заднего фона прямоугольника
    mTextItem->setColor(QColor("#0c2731")); // Цвет текста
    mTextItem->setLayer("legend"); // На один слой ниже чем this
    mTextItem->setPadding(QMargins(3, 0, 3, 0)); // Отступы от рамки прямоугольника к тексту
    mTextItem->setFont(font); // Устанавливаем базовый шрифт
    mTextItem->setSelectable(false); // Устанавливаем запрет на выбор элемента кликом

    // Создаём линию
    mLineThroughGraph = new QCPItemLine(mParentPlot);

    // Вяжем начальную точку к якорю текста
    mIntervalPos = position * 1000;


    mTextItem->position->setCoords(position, 0.9); // abs, rel
    mLineThroughGraph->end->setParentAnchor(mTextItem->position);

    mLineThroughGraph->start->setCoords(position, 1000); // abs
    mParentPlot->setInteraction(QCP::iRangeDrag, false);
    // Добавляем на верхний слой
    mLineThroughGraph->setLayer("legend"); // На один слой ниже чем this
    // Ручка для отрисовки уровней тревоги
    QPen dotPen;
    dotPen.setColor(color);
    dotPen.setStyle(Qt::DotLine);
    dotPen.setWidth(3);

    // Устанавливаем ручку
    mLineThroughGraph->setPen(dotPen);

    // Устанавливаем запрет на выбор элемента кликом
    mLineThroughGraph->setSelectable(false);
    mLineThroughGraph->setVisible(true);

    mTextItem->setSelectable(false);
}

MarkItem::~MarkItem()
{
    // Удаляем элемент текста, если есть
    if(mTextItem && mParentPlot && mParentPlot->hasItem(mTextItem)) {
        mParentPlot->removeItem(mTextItem);
    }

    // Удаляем элемент линии, если есть
    if(mLineThroughGraph && mParentPlot && mParentPlot->hasItem(mLineThroughGraph)) {
        mParentPlot->removeItem(mLineThroughGraph);
    }
}

void MarkItem::replotLine()
{
    if (isLabelCreating)
    {
        mTextItem->position->setCoords(double(mCoordLabelX)/1000.0, 0.1);
        mLineThroughGraph->start->setParentAnchor(mTextItem->position);
        mLineThroughGraph->end->setCoords(double(mCoordLabelX)/1000.0, 0);
    }
    else if (isIntervalCreating)
    {
        mTextItem->position->setCoords(double(mIntervalPos)/1000.0, 0.9);
        mLineThroughGraph->end->setParentAnchor(mTextItem->position);
        mLineThroughGraph->start->setCoords(double(mIntervalPos)/1000.0, 1000);
    }

}

void MarkItem::deleteLine()
{

    // Удаляем элемент линии, если есть
    if(mLineThroughGraph && mParentPlot && mParentPlot->hasItem(mLineThroughGraph)) {
        mParentPlot->removeItem(mLineThroughGraph);
    }

    // Удаляем элемент текста, если есть
    if(mTextItem && mParentPlot && mParentPlot->hasItem(mTextItem))
    {
        mTextItem->setText("");
        mTextItem->position->setCoords(-1, -1);
    }
}

void MarkItem::addLine()
{
    // Если метки нет или линия уже есть
    if ((!mTextItem) || mLineThroughGraph) { return; }

    // Создаём линию
    mLineThroughGraph = new QCPItemLine(mParentPlot);
    double mTempPointUpper = mParentPlot->xAxis->range().upper;
    double mTempPointLower = mParentPlot->xAxis->range().lower;


    // Вяжем начальную точку к якорю текста
    mCoordLabelX = (mTempPointLower+(mTempPointUpper-mTempPointLower)*0.1)*1000;
    mTextItem->position->setCoords(mTempPointLower+(mTempPointUpper-mTempPointLower)*0.1, 0.1); // abs, rel
    mLineThroughGraph->start->setParentAnchor(mTextItem->position);

    mLineThroughGraph->end->setCoords(mTempPointLower+(mTempPointUpper-mTempPointLower)*0.1, 0);    // abs
    mParentPlot->setInteraction(QCP::iRangeDrag, false);
    // Добавляем на верхний слой
    mLineThroughGraph->setLayer("legend"); // На один слой ниже чем this
    // Ручка для отрисовки уровней тревоги
    QPen dotPen;
    dotPen.setColor(QColor(Qt::yellow));
    dotPen.setStyle(Qt::DotLine);
    dotPen.setWidth(3);

    // Устанавливаем ручку
    mLineThroughGraph->setPen(dotPen);

    // Устанавливаем запрет на выбор элемента кликом
    mLineThroughGraph->setSelectable(false);
    mLineThroughGraph->setVisible(true);
}

void MarkItem::setVisible(bool enable)
{
    // Включаем видимость текущего айтема
    QCPAbstractItem::setVisible(enable);

    // Делаем видимым текст
    if(mTextItem) {
        mTextItem->setVisible(enable);
    }

    // Вкелючаем отображение линии через график
    if(mLineThroughGraph) {
        mLineThroughGraph->setVisible(enable);
    }
}

void MarkItem::setOrientation(LabelOrientation orientation)
{
    mOrientation = orientation;

    // Обновляем ориентацию метки
    updateMarkOrientation();
}

void MarkItem::setPositionMark(double position)
{
    if (!mTextItem) {
        return;
    }

    if(mOrientation == moVerticalBottom || mOrientation == moVerticalTop){
        mTextItem->position->setCoords(position,
                                       mTextItem->position->value());
    }
    else{
        mTextItem->position->setCoords(mTextItem->position->key(),
                                       position);
    }

    // Запоминаем текущую позицию
    mCurrentPosition = position;
}

void MarkItem::setMarkItemMargin(double level)
{
    if (!mTextItem) {
        return;
    }

    QPointF coord;
    switch (mOrientation) {
    case moVerticalBottom:
        coord = QPointF(mTextItem->position->key(), level);
        break;
    case moVerticalTop:
        coord = QPointF(mTextItem->position->key(), level);
        break;
    }
    mTextItem->position->setCoords(coord);
}

void MarkItem::setText(const QString &text)
{
    if(mTextItem){
        mTextItem->setText(text);
    }
}

double MarkItem::getMarkPosition() const
{
    return mCurrentPosition;
}

void MarkItem::updateMarkOrientation()
{
    if (!mTextItem) {
        return;
    }

    // Для установки расположения метки
    QPointF coord = QPointF(0, 0);

    switch (mOrientation) {
    case moVerticalBottom:
        mTextItem->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        mTextItem->position->setTypeX(QCPItemPosition::ptPlotCoords);
        mTextItem->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
        coord.setY(1);
        break;
    case moVerticalTop:
        mTextItem->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        mTextItem->position->setTypeX(QCPItemPosition::ptPlotCoords);
        mTextItem->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
        break;
    }

    // Устанавливаем метку на её место
    mTextItem->position->setCoords(coord);
}

double MarkItem::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(onlySelectable);

    if(mTextItem){
        double dist = mTextItem->selectTest(pos, false, details);
        return dist;
    }

    return -1;
}

void MarkItem::draw(QCPPainter *painter)
{
    Q_UNUSED(painter);
}
