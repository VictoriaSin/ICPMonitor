#ifndef DATADOWNLOADPAGE_H
#define DATADOWNLOADPAGE_H

#include "abstractdialogpage.h"
#include "usb_funcs.h"
#include "controller/controller_enums.h"

namespace Ui {
class DataDownloadPage;
}

class QButtonGroup;
class MessageDialog;
class SelectionDialog;
class BlockDevicesSelectionDialog;
class MoveDataDialog;

class DataDownloadPage : public AbstractDialogPage
{
    Q_OBJECT

    enum State {
        defaultState = 0,
        exportScreens = 1,
        exportASR = 2,
        exportCSR = 3
    };

    Ui::DataDownloadPage *ui;

    QButtonGroup *mScreenGroup {nullptr};
    QButtonGroup *mAverageSensorReadingsICPGroup {nullptr};
    QButtonGroup *mSensorReadingsGroup {nullptr};

    /*! Имена максимальных разделов флешек, их размеры, пути монтирования */
    std::vector<InfoAboutThePartOfFlashDrive> mUSBMaxPart;

    /*! Информационное окно */
    MessageDialog *mMessageDialog {nullptr};

    /*! Окно выбора флеш-накопителя */
    BlockDevicesSelectionDialog *mSelectUsbDialog {nullptr};

    /*! Окно выбора преобразования фото */
    SelectionDialog *mSelectConvertionDialog {nullptr};

    /*! Окно процесса перемещения данных */
    MoveDataDialog *mMoveDataDialog {nullptr};

    State mCurrentState = State::defaultState;

public:
    explicit DataDownloadPage(QWidget *parent = nullptr);
    ~DataDownloadPage();

private:
    /*! Экспорт текущих показаний датчика */
    void exportCurrentSensorReadings();

    /*! Экспорт средних показаний датчика */
    void exportAverageSensorReadings();

    /*! Экспорт скриншотов */
    void exportScreenshots();

    /*! Выбор флеш-накопителя
        true - флешка выбрана
        false - флешка не выбрана
    */
    bool selectFlashDrive();

    /*! Запускает следующее состояние по порядку
        в соответствии с enum State
     */
    void runNextState();

    /*! Запуск действия указанного состояния */
    void runState(State state);

private slots:
    /*! Обработка событий контроллера */
    void controllerEventHandler(ControllerEvent event, const QVariantMap &args = {});

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

public slots:
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

    // AbstractDialogPage interface
public slots:
    void done(int exodus) override;

};

#endif // DATADOWNLOADPAGE_H
