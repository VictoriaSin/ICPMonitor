#ifndef ZEROSENSORPAGE_H
#define ZEROSENSORPAGE_H

#include "abstractdialogpage.h"
#include "controller/controller_enums.h"

namespace Ui {
class ZeroSensorPage;
}

class ZeroSensorPage : public AbstractDialogPage
{
    Q_OBJECT

    /*! Отслеживание состояния датчика */
    ControllerEvent mLastSensorState = ControllerEvent::SensorDisconnected;

public:
    explicit ZeroSensorPage(QWidget *parent = nullptr);
    ~ZeroSensorPage();

private:
    Ui::ZeroSensorPage *ui;

    void setupResetSensorToolButton();

    /*! Обновление информации на странице в самом низу экрана */
    void updateInfo();

private slots:
     void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});
    /*! Тестовая, очень простая реализация запроса на обнуление сенсора */
    void testResetSensor();

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

public slots:
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

};

#endif // ZEROSENSORPAGE_H
