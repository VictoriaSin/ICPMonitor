#ifndef ALARMLEVELICPWIDGET_H
#define ALARMLEVELICPWIDGET_H

#include <QWidget>

#include "controller/controller_enums.h"

namespace Ui {
class AlarmLevelICPWidget;
}

class QSvgWidget;
class MonitorController;
class AverageICPController;

class AlarmLevelICPWidget : public QWidget
{
    Q_OBJECT

    MonitorController *mController {nullptr};

    /*! Путь к изображению включения тревоги */
    const QString AlarmEnabledSvg {":/icons/alarmEnabled.svg"};

    /*! Путь к изображению выключения тревоги */
    const QString AlarmDisabledSvg {":/icons/alarmDisabled.svg"};

    /*! Максимальный размер иконок состояний тревоги */
    const QSize MaxAlarmStateSVGSize {20, 20};


public:
    explicit AlarmLevelICPWidget(QWidget *parent = nullptr);
    ~AlarmLevelICPWidget();

    void installController(MonitorController *controller);

    void scaleFont(float scaleFactor);

private:
    /*! Настройки SVG виджетов */
    void setupSVGWidgets();

private slots:
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});


    /*! Обновление уровнений и состояний тревоги */
    void updateAlarmInfo();

    /*! Обновление нижнего и верхнего уровней тревоги на виджете */
    void updateAlarmLevelsOnWidgets();

    /*! Обновление состояний (вкл/выкл) нижнего и верхнего уровней тревоги */
    void updateAlarmStatesOnWidgets();

private:
    Ui::AlarmLevelICPWidget *ui;
    QSvgWidget *mLowLevelPressureSVG {nullptr};
    QSvgWidget *mHighLevelPressureSVG {nullptr};

public slots:

    /*! Обновление среднего значения на виджете */
    void updateAverageValueOnWidgets(double currAverage);
};

#endif // ALARMLEVELICPWIDGET_H
