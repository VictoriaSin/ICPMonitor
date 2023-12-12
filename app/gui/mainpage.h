#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "../app/ipagewidget.h"
#include "controller/controller_enums.h"
#include "blockDevices/blockdevice.h"

#include "../app/global_define.h"
#include "sensor/sensordatamanager.h"
#include "../app/plots/recordedplot.h"
#include "volumeinputpage.h"
#include "../gui/dialogWindows/patientinfodialog.h"

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
extern uint8_t mCurrentIntervalNum;
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
    QDir mCurrentRecordDir;
    VolumeInputPage *mVolumeInputPage{nullptr};
    VolumeInputPage *mParamInputPage{nullptr};

    QFile mHeadFile;
    QFile mIntervalsFile;

    PatientInfoDialog* patientInfo;
    PatientInfoDialog* sessionDirName;


private slots:
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

    /*! Обновление информации о последнем состоянии датчика */
    //void updateSensorState();

    /*! Обновляет информацию о датчике */
    //void updateSensorInfo();

    /*! Обновляет последнее время сброса датчика */
    //void updateSensorResetTime();

    /*! Обновление информации о сессии */
    //void updateSessionInfo();

    /*! Обновление количества поставленных меток */
    void updateLabelCounter();

    /*! Обновление виджета с текущим временем */
    void updateDateTime();

    /*! Обновление программного хранилища на виджетах
        isAvailable - доступно ли программное хранилище
    */
    //void updateSoftwareStorageOnWidgets(bool isAvailable);

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



    void on_zoomInterval1Button_clicked();

    void on_goBackToGraphButton_clicked();

    void on_zoomInterval2Button_clicked();

    //void on_funcFirstButton_clicked();

    //void on_funcSecondButton_clicked();

    //void on_funcThirdButton_clicked();

    //void on_dVInputButton_clicked();

    void on_dVInputButton_clicked();

    void on_funcFirstButton_clicked();

    void on_markPPointButton_clicked();

    void on_fluidInjectionButton_clicked();

    void on_acceptFluidInjectionButton_clicked();

    void on_rejectFluidInjectionButton_clicked();
    void on_goBackButton_clicked();

    void on_patientInfoButton_clicked();

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

    //VolumeInputPage *mVolumeInputPage {nullptr};
    //VolumeInputPage *mParamInputPage {nullptr};

    /*! Для отображения на экране */
    QString mLastDateTime {};

    // QWidget interface

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void stopSession();
protected slots:
    void startSession();

  signals:
    void resetWaveGraph();
    void dataReadyForRecordGraph();
    void changeCurrentGraph();
    void changeRecordedGraphInteraction(bool state);
    void changeCurrentRange(bool interval); //false=1, true=2
    void goToLabel(bool direction);
    void playBtnPressed();
    void stopRecord();
    void setAveragePointerPos(uint currAverage);
    void sessionStopped();

    // IPageWidget interface
public slots:
    void retranslate() override;
    void setAverage(uint16_t currAverage);
};

#endif // MAINPAGE_H
