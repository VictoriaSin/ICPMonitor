#ifndef ABSTRACTGRAPHAREAWIDGET_H
#define ABSTRACTGRAPHAREAWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QTimer>

#include <functional>

namespace Ui {
class AbstractGraphAreaWidget;
}

class MonitorController;

/*! Абстрактный класс для отображения графика
    с возможностью реализации нажатия кнопок для
    приближения/отдаления и передвижении графика.
    Для обновления графика необходимо реализовать
    метод replotDisplayedGraph. Базовая частота
    обновления изображения задана 50 Гц. При необходимости
    можно добавить дополнительные кнопки в левый
    лэйаут при помощи соответствующего метода.
*/
class AbstractGraphAreaWidget : public QWidget
{
    Q_OBJECT

public:
    //! Таймер для обновления области графика
    QTimer *mGuiUpdateTimer {nullptr};

protected:
    /*! Хранит размер диапазона данных по оси
            и изображения нажатия и отпускания кнопки,
            присущие данному диапазону данных.
        */
        struct IntervalAndButtonIcons {
            int interval {0};    ///< Диапазон показа времени по оси
            QIcon pressedButtonIcon {}; ///< Изображение нажатия кнопки
            QIcon defaultButtonIcon {};///< Изображение отпускания кнопки

            IntervalAndButtonIcons(int _interval, const QIcon &_buttonPressed, const QIcon &_buttonReleased) :
                interval(_interval), pressedButtonIcon(_buttonPressed), defaultButtonIcon(_buttonReleased) {}
        };



    //! Время обновления области графика по умолчанию
    int mGuiUpdate {50};

    /*! Доступные диапазоны данных (в секундах) по оси X с путями к иконкам кнопок */
    QVector<IntervalAndButtonIcons> mXRangesOfSecondsWithIcons;

    /*! Текущий выбранный диапазон по оси X */
    uint mCurrentXRangeIndex {0};

    /*! Текущий выбранный диапазон по оси Y */
    uint mCurrentYRangeIndex {0};

    /*! Контроллер */
    MonitorController *mController {nullptr};

public:
    explicit AbstractGraphAreaWidget(QWidget *parent = nullptr);
    ~AbstractGraphAreaWidget();

    /*! Установка контроллера */
    virtual void installController(MonitorController *controller);

    /*! Перевод виджета */
    virtual void retranslate();

    /*! Увеличение шрифтов */
    virtual void scaleFont(float scaleFactor);

protected:
    /*! Метод добавляет виджет после названия графика */
    bool addWidgetOnLeftVerticalLayout(QWidget *widget);

    /*! Инициализирует кнопку смены диапазона оси X.
        Для её использования необходимо перед вызовом
        метода сначала заполнить mXRangesOfSecondsWithIcons
        интересующими интервалами с изображениями нажатия и
        отпускания кнопки. Также необходимо реализовать метод
        обновления диапазона по оси X - updateXRange() и сделать
        connect.
        ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ:
        true - успешная инициализация кнопки изменения диапазона оси X;
        false - не заполнен mXRangesOfSecondsWithIcons, кнопка не инициализирована.
    */
    virtual bool initializeXRangeToolButton();

    /*! Обновление имени установленного графика */
    virtual void updateDisplayedGraphName() = 0;

private:
    /*! Настройка кнопок */
    void setupButtons();

public slots:
    /*! Запускает обновление графика */
    virtual bool startPlotting();

    /*! Останавливает обновление графика */
    virtual bool stopPlotting();

protected slots:
    /*! Перерисовывает текущий отображаемый график */
    virtual void replotDisplayedGraph() = 0;

    /*! Смена диапазона по оси X */
    virtual bool nextXRange() = 0;

    /*! Смена диапазона по оси Y */
    virtual void nextYRange() = 0;

protected:
    Ui::AbstractGraphAreaWidget *ui;

    // QWidget interface
protected:
    /*! При показе виджета запускает отрисовку графика */
    virtual void showEvent(QShowEvent *event) override;

    /*! При скрытии виджета останавливает отрисовку графика */
    virtual void hideEvent(QHideEvent *event) override;

};

#endif // ABSTRACTGRAPHAREAWIDGET_H
