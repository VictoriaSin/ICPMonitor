#ifndef TECHNICALACCESSPAGE_H
#define TECHNICALACCESSPAGE_H

#include "abstractdialogpage.h"
#include "controller/controller_enums.h"

#include <QIcon>

class BlockDevicesSelectionDialog;

namespace Ui {
class TechnicalAccessPage;
}

class TechnicalAccessPage : public AbstractDialogPage
{
    Q_OBJECT

    Ui::TechnicalAccessPage *ui;

    /*! Окно выбора флеш-накопителя */
    BlockDevicesSelectionDialog *mSetSoftwareStorageDialog {nullptr};

    /*! Кнопка назад */
    const QIcon BackButtonIcon {":/icons/settings.svg"};
    const QIcon BackButtonIconPressed {":/icons/settings_pressed.svg"};

public:
    explicit TechnicalAccessPage(QWidget *parent = nullptr);
    ~TechnicalAccessPage();

private:
    /*! Настройка спинбокса с вводом макс. кол-ва скриншотов */
    void setupMaxScreensSpinBox();

    /*! Обновление информации диалогового
        окна выбора программного хранилища
    */
    void updateSelectSoftwareStorageDialog();

    /*! Обновление строки текущего программного
        хранилища
    */
    void updateCurrentSoftwareStorageLabel();

    /*! Обновление строки с максимальным кол-вом скриншотов */
    void updateMaxScreensLabel();

    /*! Обновление максимального кол-ва скриншотов */
    void updateMaxScreens();

private slots:
    /*! Обработка событий контроллера */
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

    /*! Запуск окна выбора программного хранилища */
    void openSoftwareStorageDialog();

    /*! Обработка выбора програмнного хранилища */
    void closedSoftwareStorageDialog(int result);

    // IPageWidget interface
public:
    void scaleFont(float scaleFactor) override;
    void installController(MonitorController *controller) override;

public slots:
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

    // AbstractDialogPage interface
public slots:
    void done(int exodus) override;
};

#endif // TECHNICALACCESSPAGE_H
