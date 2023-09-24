#include "intervalplot.h"
#include "recordedplot.h"
#include "gui/gui_funcs.h"

IntervalPlot::IntervalPlot(QWidget *parent):
    AbstractCustomPlot(GraphType::IntervalGraph, parent)
{
    mMainGraph = addGraph();
    mMainGraph->setAntialiased(true);
    addLayer("lineLayer", nullptr, limAbove);
    layer("lineLayer")->setMode(QCPLayer::lmBuffered);
    mICPSettings->getCurrentPressureIndex() == 0 ? mCurrentMaxYRange = 60 : mCurrentMaxYRange = 815.7;
    retranslate();
}

//IntervalPlot::~IntervalPlot()
//{
//    //
//}

void IntervalPlot::setup(QVector<QPair<double, double>> dataVector, QColor color)
{
    //настройка ручки
    QBrush brush;
    brush.setStyle(Qt::NoBrush);

    // Подготавливаем ручку
    QPen pen;
    pen.setColor(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2); // толщина линии

    // Устанавливаем браш и ручку для отрисовки основного графика
    mMainGraph->setPen(pen);
    mMainGraph->setBrush(brush);

    for (uint16_t i=0; i<dataVector.count(); i++)
    {
        mMainGraph->addData(dataVector[i].first, dataVector[i].second);
    }
    xAxis->setRange(dataVector[0].first, dataVector[dataVector.count()-1].first);

}


void IntervalPlot::retranslate()
{
    // Подписываем оси
    xAxis->setLabel(tr("секунды"));
    mICPSettings->getCurrentPressureIndex() == 0 ? yAxis->setLabel(tr("мм рт ст")) : yAxis->setLabel(tr("мм вод ст"));

}

void IntervalPlot::scaleFont(double scaleFactor)
{
    WFontGraphScaling(this, scaleFactor);
}

//bool IntervalPlot::event(QEvent *event)
//{
//    // сенсор
//}
