#ifndef AVERAGEICPWIDGET_H
#define AVERAGEICPWIDGET_H

#include <QWidget>
#include <QVariantMap>

#include "controller/controller_enums.h"

namespace Ui {
class AverageICPWidget;
}

class MonitorController;
class AverageICPController;

class AverageICPWidget : public QWidget
{
    Q_OBJECT

    MonitorController *mController {nullptr};

    /*! Буфер для преобразования числа в текст */
    char *mBufferAverageNumber {nullptr};

    /*! Размер буффера */
    int mBufferSize {7};

public:
    explicit AverageICPWidget(QWidget *parent = nullptr);
    ~AverageICPWidget();

    void installController(MonitorController *controller);

    void scaleFont(float scaleFactor);

    void retranslate();

private slots:
    void updateAverage();
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

private:
    Ui::AverageICPWidget *ui;
};

#endif // AVERAGEICPWIDGET_H
