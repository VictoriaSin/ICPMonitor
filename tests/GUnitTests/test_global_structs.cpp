#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "global_structs.h"

// Поведение для равных значений не важно
TEST(ComplexValueTestSuit, timestampsComparison) {
    ComplexValue v1;
    ComplexValue v2;

    v1.timestamp = 0;
    v2.timestamp = -1;
    EXPECT_TRUE(ComplexValue::timestampGreaterThan(v1, v2));
    EXPECT_FALSE(ComplexValue::timestampLessThan(v1, v2));

    v1.timestamp = -1;
    v2.timestamp = 0;
    EXPECT_FALSE(ComplexValue::timestampGreaterThan(v1, v2));
    EXPECT_TRUE(ComplexValue::timestampLessThan(v1, v2));
}

// Поведение для равных значений не важно
TEST(ComplexValueTestSuit, valuesComparison) {
    ComplexValue v1;
    ComplexValue v2;

    v1.value = 0.0;
    v2.value = -1.0;
    EXPECT_TRUE(ComplexValue::valueGreaterThan(v1, v2));
    EXPECT_FALSE(ComplexValue::valueLessThan(v1, v2));

    v1.value = -1.0;
    v2.value = 0.0;
    EXPECT_FALSE(ComplexValue::valueGreaterThan(v1, v2));
    EXPECT_TRUE(ComplexValue::valueLessThan(v1, v2));
}
