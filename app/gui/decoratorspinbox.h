#ifndef DECORATORSPINBOX_H
#define DECORATORSPINBOX_H

#include <QWidget>

class QAbstractSpinBox;
class QPushButton;
class QHBoxLayout;

/*! Данный класс добавляет по обе стороны спинбокса
    кнопки "Уменьшить значение" и "Увеличить значение".
    Способен хранить в себе QDateTimeWidget, QSpinBox, QDoubleSpinBox.
 */
class DecoratorSpinBox : public QWidget
{
    Q_OBJECT

    QAbstractSpinBox *mCurrentSpinBox {nullptr};
    QPushButton *mMinusButton {nullptr};
    QPushButton *mPlusButton {nullptr};
    QHBoxLayout *mHBoxLayout {nullptr};

public:
    explicit DecoratorSpinBox(QWidget *parent = nullptr);
    ~DecoratorSpinBox();

    /*! Возвращает установленный SpinBox */
    QAbstractSpinBox* currentSpinBox() const;

    /*! Устанавливает один из трёх виджетов: QDateTimeEdit, QSpinBox, QDoubleSpinBox.
        Происходит передача владения виджетом. (Будет удалён при установке следующего виджета)
     */
    bool setSpinBox(QAbstractSpinBox *spinBox);

    /*! Устанавливает для QSpinBox или QDoubleSpinBox диапазон допустимых значений */
    bool setRange(double minimum, double maximum);

    /*! Установка текущего значения */
    bool setValue(double value);

    /*! Возврат текущего значения QSpinBox или QDoubleSpinBox
        ok - удалось ли получить значение
     */
    double value(bool *ok) const;

    /*! Установка времени для QDateTimeEdit */
    bool setTime(const QTime &time);

    /*! Установка даты для QDateTimeEdit */
    bool setDate(const QDate &date);

    /*! Возврат установленного времени
        ok - удалось ли получить значение
     */
    QTime time(bool *ok) const;

    /*! Возврат установленной даты
        ok - удалось ли получить значение
     */
    QDate date(bool *ok) const;
};

#endif // DECORATORSPINBOX_H
