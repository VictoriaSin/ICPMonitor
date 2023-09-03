#ifndef SCALING_H
#define SCALING_H

#include <QFont>

class AbstractCustomPlot;

QFont FontScaling(QFont &font, float factor);
void WFontScaling(QWidget *w, float factor);
void WFontGraphScaling(AbstractCustomPlot *graph, float scaleFactor);
QPixmap PixmapScaling(QPixmap pixmap, float scaleFactor);

/*! Делает скришот виджета и возвращает его */
QPixmap makeAScreenOfWidget(QWidget *widget);

/*! Чтение стиля виджета из файла */
QString readStyleSheetFile(const QString &styleSheetFile);

#endif // SCALING_H
