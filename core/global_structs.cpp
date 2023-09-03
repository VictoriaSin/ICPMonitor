#include "global_structs.h"

bool ComplexValue::timestampLessThan(const ComplexValue &v1, const ComplexValue &v2)
{
    return v1.timestamp < v2.timestamp;
}

bool ComplexValue::timestampGreaterThan(const ComplexValue &v1, const ComplexValue &v2)
{
    return v1.timestamp > v2.timestamp;
}

bool ComplexValue::valueLessThan(const ComplexValue &v1, const ComplexValue &v2)
{
    return v1.value < v2.value;
}

bool ComplexValue::valueGreaterThan(const ComplexValue &v1, const ComplexValue &v2)
{
    return v1.value > v2.value;
}

void ComplexValue::clear()
{
    value = 0;
    timestamp = 0;
    valid = false;
}
