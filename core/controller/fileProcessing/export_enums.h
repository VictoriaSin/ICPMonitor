#ifndef EXPORT_ENUMS_H
#define EXPORT_ENUMS_H

#include "ultrafastconverting/countlut.h"
#include "ultrafastconverting/dateconversion.h"

#include <cstdint>

/*! Перечисление функций экспорта даты */
enum class ExportDataFunc {
    TimeStamp,          ///< Время в формате TimeStamp (Быстро)
    DateTimeStrfTime,   ///< Дата вычисляется при помощи StrfTime
    DateTimeCountLut,   ///< Дата вычисляется при помощи функции countLut
    DateTimeTrained     ///< Дата вычисляется при помощи запомненных значений дней и годов
};

/*! Конвертация даты в зависимости от выбранной функции
    Возвращает кол-во байтов, записанных в buffer.
    func - выбор функции экспорта даты;
    timestampMs - время, необходимое для конвертации в строку;
    buffer - куда пишем результат работы функции.
*/
int dateConverterFunc(ExportDataFunc func, int64_t timestampMs, char *buffer);

/*! Данные, необходимые для экспорта */
enum class ExportValues : uint32_t {
    IndicationTime = 1, ///< Время показания
    Indication = 2,     ///< Показание
    Validity = 4,       ///< Валидность
    AlarmState = 8,     ///< Состояние тревоги
    UserLabel = 16,     ///< Пользовательская метка
    UserInfoLabel = 32, ///< Информация о пользовательской метке
    //AutomaticLabel = 64  ///< Автоматические метки (генерируемые алгоритмом анализа)
};

inline uint32_t operator|(ExportValues a, ExportValues b)
{
    return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

inline uint32_t operator|(uint32_t a, ExportValues b)
{
    return a | static_cast<uint32_t>(b);
}

inline uint32_t operator&(uint32_t a, ExportValues b)
{
    return a & static_cast<uint32_t>(b);
}

inline uint32_t operator==(uint32_t a, ExportValues b)
{
    return a == static_cast<uint32_t>(b);
}

#endif // EXPORT_ENUMS_H
