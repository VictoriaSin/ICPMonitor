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
double calculateMedian(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted = false);

/*! Расчёт нижнего квартиля
    ВХОДЯЩИЕ ПАРАМЕТРЫ:
    data - указатель на данные (будут отсортированы по возрастанию значения, если удастся посчитать нижний квартиль);
*/
double calculateLowerQuartile(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted = false);

/*! Расчёт верхнего квартиля
    ВХОДЯЩИЕ ПАРАМЕТРЫ:
    data - указатель на данные (будут отсортированы по возрастанию значения, если удастся посчитать верхний квартиль);
*/
double calculateUpperQuartile(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted = false);

#endif // STATISTICAL_FUNCTIONS_H
