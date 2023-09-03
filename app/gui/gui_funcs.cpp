#include "gui_funcs.h"
#include "plots/abstractcustomplot.h"

QFont FontScaling(QFont &font, float scaleFactor)
{
    font.setPointSizeF(font.pointSizeF()*scaleFactor);
    return font;
}

void WFontScaling(QWidget *w, float scaleFactor)
{
    QFont font = w->font();
    w->setFont(FontScaling(font, scaleFactor));
}

void WFontGraphScaling(AbstractCustomPlot *graph, float scaleFactor)
{
    // Получаем шрифты надписей на тиках
    QFont fontX = graph->xAxis->tickLabelFont();
    QFont fontX2 = graph->xAxis2->tickLabelFont();
    QFont fontY = graph->yAxis->tickLabelFont();
    QFont fontY2 = graph->yAxis2->tickLabelFont();

    // Увеличиваем надписи у меток деления
    FontScaling(fontX, scaleFactor);
    graph->xAxis->setTickLabelFont(fontX);
    FontScaling(fontX2, scaleFactor);
    graph->xAxis2->setTickLabelFont(fontX2);
    FontScaling(fontY, scaleFactor);
    graph->yAxis->setTickLabelFont(fontY);
    FontScaling(fontY2, scaleFactor);
    graph->yAxis2->setTickLabelFont(fontY2);

    // Получаем шрифт меток осей
    fontX = graph->xAxis->labelFont();
    fontX2 = graph->xAxis2->labelFont();
    fontY = graph->yAxis->labelFont();
    fontY2 = graph->yAxis2->labelFont();

    // Увеличиваем подписи к осям
    FontScaling(fontX, scaleFactor);
    graph->xAxis->setLabelFont(fontX);
    FontScaling(fontX2, scaleFactor);
    graph->xAxis2->setLabelFont(fontX2);
    FontScaling(fontY, scaleFactor);
    graph->yAxis->setLabelFont(fontY);
    FontScaling(fontY2, scaleFactor);
    graph->yAxis2->setLabelFont(fontY2);
}

QPixmap PixmapScaling(QPixmap pixmap, float scaleFactor)
{
    QSize currentSize = pixmap.size();
    return pixmap.scaled(currentSize.width() * scaleFactor, currentSize.height() * scaleFactor);
}

QPixmap makeAScreenOfWidget(QWidget *widget)
{
    if (!widget)
        return QPixmap();

    QPixmap screen(widget->size());
    widget->render(&screen);

    return screen;
}

QString readStyleSheetFile(const QString &styleSheetFile)
{
    QFile file(styleSheetFile);
    if (!file.exists()) {
        return {};
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const QString styleSheet = file.readAll();
    file.close();
    return styleSheet;
}
