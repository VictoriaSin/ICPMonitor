#ifndef MAINMENU_H
#define MAINMENU_H

#include "abstractmenupage.h"
#include "controller/controller_enums.h"

class AlarmSetupPage;
class SystemInfoPage;
class ZeroSensorPage;
//class DataDownloadPage;
class DateTimePage;
class LanguagePage;
class GeneralSettingsPage;
class ExportDataPage;

class MainMenu : public AbstractMenuPage
{
    Q_OBJECT

    /*! ID кнопок, ведущих на страницы */
    enum PageID : uint8_t {
        SystemInfo = 0,
        ZeroSensor = 1,
        //DataExport = 2,
        DateTime = 3,
        Language = 4,
        General = 5,
        ExportData = 6,
        PowerOff = 7
    };

    SystemInfoPage *mSystemInfoPage {nullptr};
    ZeroSensorPage *mZeroSensorPage {nullptr};
    //DataDownloadPage *mDataExportPage {nullptr};
    DateTimePage *mDateTimePage {nullptr};
    LanguagePage *mLanguagePage {nullptr};
    GeneralSettingsPage *mGeneralSettingsPage {nullptr};
    ExportDataPage *mExportDataPage {nullptr};

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

private slots:
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

public:
    void scaleFont(float scaleFactor) override;
    void installController(MonitorController *controller) override;

    // IPageWidget interface
public slots:
    void retranslate() override;
protected slots:
    void powerOff(int id);
};

#endif // MAINMENU_H
