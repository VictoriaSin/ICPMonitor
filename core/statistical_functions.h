#ifndef STATISTICAL_FUNCTIONS_H
#define STATISTICAL_FUNCTIONS_H

#include <QVector>

class ComplexValue;

/*################################################################################
                                Коробчатая диаграмма
################################################################################*/
/*! Расчёт медианы
    ВХОДЯЩИЕ ПАРАМЕТРЫ:
    data - указатель на данные (будут отсортированы по возрастанию значения, если удастся посчитать медиану);
*/
float calculateMedian(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted = false);

/*! Расчёт нижнего квартиля
    ВХОДЯЩИЕ ПАРАМЕТРЫ:
    data - указатель на данные (будут отсортированы по возрастанию значения, если удастся посчитать нижний квартиль);
*/
float calculateLowerQuartile(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted = false);

/*! Расчёт верхнего квартиля
    ВХОДЯЩИЕ ПАРАМЕТРЫ:
    data - указатель на данные (будут отсортированы по возрастанию значения, если удастся посчитать верхний квартиль);
*/
float calculateUpperQuartile(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted = false);

#endif // STATISTICAL_FUNCTIONS_H
