#ifndef GLOBAL_STRUCTS_H
#define GLOBAL_STRUCTS_H

#include <QObject>

/*! Значение данных с привязкой ко времени
    и флагом валидности
 */
struct ComplexValue{
    float value {0};          ///< Показание
    int64_t timestamp {0};     ///< Время получения значения, ms

    bool valid {false};        ///< Валидность/корректность значения

    /*! Очищает значения структуры */
    void clear();

    /*! Меньшее значение по времени */
    static bool timestampLessThan(const ComplexValue &v1, const ComplexValue &v2);

    /*! Большее значение по времени */
    static bool timestampGreaterThan(const ComplexValue &v1, const ComplexValue &v2);

    /*! Меньшее значение по показанию */
    static bool valueLessThan(const ComplexValue &v1, const ComplexValue &v2);

    /*! Большее значение по показанию */
    static bool valueGreaterThan(const ComplexValue &v1, const ComplexValue &v2);
};

#endif // GLOBAL_STRUCTS_H
