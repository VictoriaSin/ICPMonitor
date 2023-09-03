#ifndef IPAGEWIDGET_H
#define IPAGEWIDGET_H

#include <QWidget>

class MonitorController;
class MainWindow;

class IPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IPageWidget(QWidget *parent = nullptr);
    virtual void scaleFont(float scaleFactor) = 0;

    virtual void installController(MonitorController *controller) = 0;

public slots:
    virtual void retranslate() = 0;

signals:
    void changePage(IPageWidget *installedPage);
    void previousPage();
};

#endif // IPAGEWIDGET_H
