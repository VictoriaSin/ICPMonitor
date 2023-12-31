#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStack>
#include "controller/controller_enums.h"
#include <QThread>
#include "controller/monitorcontroller.h"
#include "gui/zerosensorpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainPage;
class IPageWidget;
class MonitorController;
class MessageDialog;


class MainWindow : public QWidget
{
    Q_OBJECT

    Ui::MainWindow *ui;
    IPageWidget *mCurrentPage {nullptr};
    QStack<IPageWidget*> mStackOfWidgets;

    /*! Диалоговое окно для уведомлений пользователя */
    //MessageDialog *mMessageDialog {nullptr};

public:
    QThread mControllerThread {nullptr};
    MonitorController *mController {nullptr};
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void installController(MonitorController *controller);
    /*! Скейлинг шрифтов */
    void scaleFonts();
    void setZeroSensorPage();
  public slots:
    void destroyMonitorController();
  private:
    /*! Изменяет текущую страницу
        installedPage - указатель на устанавливаемую страницу
        Возвращает значения:
        true - страница была установлена успешно
        false - страница не была установлена
     */
    bool changeCurrentPage(IPageWidget *installedPage);

private slots:
    /*! Обработка событий контроллера */
    void controllerEventHandler(ControllerEvent event, const QVariantMap &args = {});
    //! Замена текущей страницы на устанавливаемую installedPage
    void setPage(IPageWidget *installedPage);
    //! Установка предыдущей страницы
    void setPreviousPage();
protected:
    void changeEvent(QEvent *) override;
};
#endif // MAINWINDOW_H
