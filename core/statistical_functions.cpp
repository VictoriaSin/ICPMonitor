#include "statistical_functions.h"
#include "global_structs.h"

float calculateLowerQuartile(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted)
{
    // Если пришёл nullptr
    if (!values) {
        isOkey = false;
        return -1;
    }

    // Кол-во элементов в массиве
    const int countElements = values->size();

    // Если значений равно одному или меньше 5
    if (countElements == 1 || countElements < 5) {
        isOkey = false;
        return -1;
    }

    // Сортируем по величине в порядке возрастания
    if (!isSorted) {
        std::sort(values->begin(), values->end(), ComplexValue::valueLessThan);
    }

    // Возвращает медиану нижнего квартиля
    std::vector<ComplexValue> temp(values->begin(), values->begin() + countElements / 2);
    return calculateMedian(&temp, isOkey, true);
}

float calculateUpperQuartile(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted)
{
    // Если пришёл nullptr
    if (!values) {
        isOkey = false;
        return -1;
    }

    // Кол-во элементов в массиве
    const int countElements = values->size();

    // Если значений равно одному или меньше 5
    if (countElements == 1 || countElements < 5) {
        isOkey = false;
        return -1;
    }

    // Сортируем по велечине в порядке возрастания
    if (!isSorted) {
        std::sort(values->begin(), values->end(), ComplexValue::valueLessThan);
    }

    // Возвращает медиану верхнего квартиля
    std::vector<ComplexValue> temp = countElements % 2 == 0 ?
                std::vector<ComplexValue>(values->begin() + countElements / 2, values->end()) :
                std::vector<ComplexValue>(values->begin() + countElements / 2 + 1, values->end());
    return calculateMedian(&temp, isOkey, true);
}

float calculateMedian(std::vector<ComplexValue> *values, bool &isOkey, bool isSorted)
{
    // Если пришёл nullptr или значений нет
    if (!values || values->size() == 0) {
        isOkey = false;
        return -1;
    }

    // Флаг успешного вычисления медианы
    isOkey = true;

    // Сортируем по велечине в порядке возрастания
    if (!isSorted) {
        std::sort(values->begin(), values->end(), ComplexValue::valueLessThan);
    }

    // Кол-во элементов в массиве
    const int countElements = values->size();

    // Иначе число элементов нечётное
    return countElements % 2 == 0 ? (values->at(countElements / 2).value + values->at(countElements / 2 - 1).value) / 2 : values->at(countElements / 2).value;
}
