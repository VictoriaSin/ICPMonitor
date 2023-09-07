#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include "abstractdialogwindow.h"

namespace Ui {
class SelectionDialog;
}

class QButtonGroup;
class QAbstractButton;
class QToolButton;
class QRadioButton;

class SelectionDialog : public AbstractDialogWindow
{
    Q_OBJECT

    /*! Шаблонный стиль с установкой цвета выделения ToolButton */
    const QString TemplateHighlightToolButtonStyle = "QToolButton{"
                                    "spacing: 6px;"
                                    "color: #f0f0f0f0;"
                                    "border: 3px solid black;"
                                    "border-radius: 15px;"
                                    "background-color: #%1"
                                    "}";

    /*! Неизменяемый стиль кнопок */
    const QString BaseStyleForButtons = "QToolButton{"
                                        "spacing: 6px;"
                                        "color: white;"
                                        "border: 3px solid black;"
                                        "border-radius: 15px;"
                                        "background-color: #206986;"
                                        "}"

                                        "QRadioButton{"
                                        "spacing: 6px;"
                                        "color: #f0f0f0f0;"
                                        "}"

                                        "QRadioButton::indicator {"
                                        "width: 100px;"
                                        "height: 50px;"
                                        "}"

                                        "QRadioButton{"
                                        "spacing: 6px;"
                                        "color: #f0f0f0f0;"
                                        "}"

                                        "QRadioButton::indicator:checked {"
                                        "image: url(:/icons/radioButtonSelected.svg);"
                                        "}"

                                        "QRadioButton::indicator:unchecked"
                                        "{"
                                        "image: url(:/icons/radioButtonUnselected.svg);"
                                        "}";

    /*! Текущий стиль выделения кнопок */
    QString mCurrentHighlightToolButtonStyle;

protected:
    Ui::SelectionDialog *ui;

    /*! Контейнер с кнопками выбора */
    QButtonGroup *mContainerButtons {nullptr};

    QPushButton *mAcceptButton {nullptr};
    QPushButton *mRejectButton {nullptr};

    /*! Последняя нажатая кнопка */
    QAbstractButton *mLastClikedButton {nullptr};

    QFont mFontForText = QFont("Sans Serif", 30);

public:
    explicit SelectionDialog(QWidget *parent = nullptr);
    ~SelectionDialog();

    /*! Добавление туллбаттона как элемента выбора пользователя.
        Возвращает указатель на созданную кнопку.

        Параметры:
        nameLabel - текст, располагаемый справа от кнопки
        icon - картинка, располагаемая на кнопке, слева от текста
        id - id кнопки
        sizeIcon - размер отображаемой картинки
        row/col - строка/столбец, в которую помещаем кнопку
     */
    QToolButton *addToolButton(const QString &nameLabel, const QIcon &icon, const QSize &sizeIcon, int id, int row, int col);

    /*! Добавление радиобаттона как элемента выбора пользователя.
        Возвращает указатель на созданную кнопку.

        Параметры:
        nameLabel - текст, располагаемый справа от кнопки
        icon - картинка, располагаемая на кнопке, слева от текста
        id - id кнопки
        sizeIcon - размер отображаемой картинки
        row/col - строка/столбец, в которую помещаем кнопку
     */
    QRadioButton *addRadioButton(const QString &nameLabel, const QIcon &icon, const QSize &sizeIcon, int id, int row, int col);

    /*! Возвращает группу кнопок */
    const QButtonGroup *getButtons() const;

    /*! Возвращает id выбранной кнопки */
    int getSelectedId() const;

    /*! Установка текста сверху */
    void setInfoText(const QString &info);

    /*! Устанавливает цвет выделения кнопки после нажатия
        color - цвет в формате rrggbb (Пример: F0F0F0)
     */
    void setToolButtonHighlightColor(const QString& hexColor);

    /*! Удаляет все созданные кнопки */
    void removeButtons();

    /*! Возвращает кнопку "Продолжить" */
    QPushButton *getAcceptButton() const;

    /*! Возвращает кнопку "Отменить" */
    QPushButton *getRejectButton() const;

protected:
    /*! Сброс выбора */
    void resetSelection();

protected slots:
    /*! Выделяет тулбаттон, снимая выделение с ранее выделенных тулбаттонов */
    void highlightToolButton(QAbstractButton *button);
    void highlightToolButton(int id);

    /*! Снимает выделение со всех тулбаттонов */
    void resetHighlightToolButtons();

    /*! Устанавливаем последнюю нажатую кнопку */
    void setLastClickedButton(QAbstractButton *button);

private:
    /*! Добавление любой кнопки */
    QAbstractButton *addRadioOrToolButton(QAbstractButton *button, const QString &nameLabel, const QIcon &icon, const QSize &sizeIcon, int id, int row, int col);

    // AbstractDialogWindow interface
public:
    void scaleFont(float scaleFactor) override;
    void retranslate() override;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

    // QDialog interface
public slots:
    /*! Отменяет изменения при нажатии на кнопку отмены */
    void reject() override;
};

#endif // SELECTIONDIALOG_H
