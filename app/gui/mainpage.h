#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "ipagewidget.h"
#include "controller/controller_enums.h"
#include "blockDevices/blockdevice.h"

#include "global_define.h"
#include "sensor/sensordatamanager.h"
#include "sensor/savespi.h"
#include "plots/recordedplot.h"
namespace Ui {
class MainPage;
}

class MainMenu;
class AbstractGraphAreaWidget;
class CurrentGraphsArea;
class StatisticGraphsArea;
class LabelMarkItem;
class BlockDevice;
class MarkItem;
class Label;
class RecordedPlot;

extern bool isPlayRecord;

enum speed { SpeedX1 = 500, SpeedX2 = 250, SpeedX4 = 125};

class MainPage : public IPageWidget
{
    Q_OBJECT

    /*! Таймер для обновления виджета с текущим временем */
    QTimer *mUpdateDateTimeTimer {nullptr};

    /*! Отслеживание состояния датчика */
    ControllerEvent mLastSensorState = ControllerEvent::SensorDisconnected;

    bool isStart = true;

    bool isSessionStart = true;

    bool isStartInterval = true;

    int iconCount = 0;
    SensorDataManager *mSensorDataManager;
    SaveSPI *mSPIFile;
    //bool isPlayRecord = true;

public:
    //!!!! Была приватной
    /*! Кнопка главного меню */
    MainMenu *mMainMenu {nullptr};
    explicit MainPage(QWidget *parent = nullptr);
    ~MainPage();

    void installController(MonitorController *controller) override;

    void scaleFont(float scaleFactor) override;

private:
    /*! Настройка кнопок */
    void setupButtons();

    /*! Настройка нижних иконок-уведомлений */
    void setupBottomInfoSVG();

    /*! Настройка окна главного меню */
    void setupMainMenu();

    /*! Настройка контейнера с графиками */
    void setupGraphsContainer();

private slots:
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

    /*! Обновление информации о последнем состоянии датчика */
    void updateSensorState();

    /*! Обновляет информацию о датчике */
    void updateSensorInfo();

    /*! Обновляет последнее время сброса датчика */
    void updateSensorResetTime();

    /*! Обновление информации о сессии */
    void updateSessionInfo();

    /*! Обновление количества поставленных меток */
    void updateLabelCounter();

    /*! Обновление виджета с текущим временем */
    void updateDateTime();

    /*! Обновление программного хранилища на виджетах
        isAvailable - доступно ли программное хранилище
    */
    void updateSoftwareStorageOnWidgets(bool isAvailable);

    /*! Скриншот текущего экрана */
    //void makeScreen();

    /*! Смена графика */
    void nextGraph();

    /*! Подтверждает постановку метки*/
    void on_acceptMarkButton_clicked();

    /*! Отменяет постановку метки*/
    void on_rejectMarkButton_clicked();

    /*! Открывает меню */
    void on_homeButton_clicked();

    /*! Старт/стоп записи*/
    void on_recordButton_clicked();

    /*! Создание метки */
    void on_makeLabelButton_clicked();

    /*! Начинает сессию*/
    void on_sessionButton_clicked();

    void on_intervalButton_clicked();

    void on_acceptIntervalButton_clicked();

    void on_rejectIntervalButton_clicked();

    void on_goToInterval1Button_clicked();

    void on_goToInterval2Button_clicked();

    void on_goToPreviousMarkButton_clicked();

    void on_goToNextMarkButton_clicked();

    void on_playRecord_clicked();

    void on_rewindRecordButton_clicked();

    void on_downloadGraphButton_clicked();

    void on_speedRecordButton_clicked();

    void setAverage(double currAverage);

private:
    Ui::MainPage *ui;

    /*! Контроллер приложения */
    MonitorController *mController {nullptr};

    /*! Область с текущими графиками */
    CurrentGraphsArea *mCurrentGraphsArea {nullptr};

    /*! Область со статистическими графиками */
    StatisticGraphsArea *mStatisticGraphsArea {nullptr};

    /*! Контейнер графиков */
    QVector<AbstractGraphAreaWidget *> mGraphWidgetContainer;

    /*! Номер текущего графика */
    uint8_t mGraphNumber = 0;



    /*! Для отображения на экране */
    QString mLastDateTime {};

    // QWidget interface

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    void startSession();
    void stopSession();
  signals:
    void resetWaveGraph();
    void dataReadyForRecordGraph();
    void changeCurrentGraph();
    void changeRecordedGraphInteraction(bool state);
    void changeCurrentRange(bool interval); //false=1, true=2
    void goToLabel(bool direction);
    void playBtnPressed();
    void stopRecord();
    void setAveragePointerPos(double currAverage);

    // IPageWidget interface
public slots:
    void retranslate() override;
};

#endif // MAINPAGE_H
