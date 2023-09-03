#ifndef SYSTEMINFOPAGE_H
#define SYSTEMINFOPAGE_H

#include "memory"

#include "abstractdialogpage.h"
#include "controller/controller_enums.h"

namespace Ui {
class SystemInfoPage;
}

class QProcess;
class BlockDevice;

class SystemInfoPage : public AbstractDialogPage
{
    Q_OBJECT

    Ui::SystemInfoPage *ui;

    /*! Таймер обновления шкал памяти */
    QTimer *mUpdateMemBarsTimer {nullptr};

    /*! Интервал обновления шкал памяти */
    const int IntervalUpdateMemBarsMs {1000};

    //! Шаблон для вывода кол-ва доступной памяти
    const QString TemplateMemoryInfo {"%1 МБ %2 / %3 МБ"};

    /*! Программное хранилище */
    std::shared_ptr<BlockDevice> mSoftwareStorage;

public:
    explicit SystemInfoPage(QWidget *parent = nullptr);

    ~SystemInfoPage();

private:
    /*! Устанавливает шкалу программного хранилища пустой */
    void softwareStorageUnavailable();

public slots:
    /*! Обновление шкал памяти */
    void updateProgressBars();

    /*! Обновляет шкалу оперативной памяти */
    void updateRAMProgressBar();

    /*! Обновляет шкалу памяти программного хранилища */
    void updateSoftwareStorageProgressBar();

private slots:
    void controllerEventHandler(ControllerEvent event);//, const QVariantMap &args = {});

    // IPageWidget interface
public:
    void installController(MonitorController *controller) override;
    void scaleFont(float scaleFactor) override;

public slots:
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

};

#endif // SYSTEMINFOPAGE_H
