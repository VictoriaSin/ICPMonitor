#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <limits>
#include "ultrafastconverting/countlut.h"

TEST(CountLutsTestSuit, u32toaCountulTest) {
    // NULLPTR
    EXPECT_EXIT({int expect = u32toa_countlut(5, nullptr); // Ожидаем получить сдвиг равный 0
                 if (expect == 0) {
                     fprintf(stderr, "PASSED nullptr");
                 }
                 exit(0);}, ::testing::ExitedWithCode(0), "PASSED nullptr");

    // MAX VALUE
    char *buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[10] = '\0';
    const int shift = u32toa_countlut(4294967295, buffer);
    EXPECT_EQ(10, shift); // Сдвиг должен быть равен 10
    EXPECT_STREQ("4294967295", buffer); // Буфер должен содержать "4294967295\0"
    delete [] buffer;

    // POSITIVE VALUE
    buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[10] = '\0';
    const int shift2 = u32toa_countlut(1234567890, buffer);
    EXPECT_EQ(10, shift2); // Сдвиг должен быть равен 10
    EXPECT_STREQ("1234567890", buffer); // Буфер должен содержать "1234567890\0"
    delete [] buffer;

    // MIN VALUE
    buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[1] = '\0';
    const int shift3 = u32toa_countlut(0, buffer);
    EXPECT_EQ(1, shift3); // Сдвиг должен быть равен 1
    EXPECT_STREQ("0", buffer); // Буфер должен содержать "0\0"
    delete [] buffer;
}

TEST(CountLutsTestSuit, i32toaCountulTest) {
    // NULLPTR
    EXPECT_EXIT({int expect = i32toa_countlut(5, nullptr); // Ожидаем получить сдвиг равный 0
                 if (expect == 0) {
                     fprintf(stderr, "PASSED nullptr");
                 }
                 exit(0);}, ::testing::ExitedWithCode(0), "PASSED nullptr");

    // MAX VALUE
    char *buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[10] = '\0';
    const int shift = i32toa_countlut(2147483647, buffer);
    EXPECT_EQ(10, shift); // Сдвиг должен быть равен 10
    EXPECT_STREQ("2147483647", buffer); // Буфер должен содержать "2147483647\0"
    delete [] buffer;

    // POSITIVE VALUE
    buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[10] = '\0';
    const int shift2 = i32toa_countlut(1234567890, buffer);
    EXPECT_EQ(10, shift2); // Сдвиг должен быть равен 10
    EXPECT_STREQ("1234567890", buffer); // Буфер должен содержать "1234567890\0"
    delete [] buffer;

    // ZERO VALUE
    buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[1] = '\0';
    const int shift3 = i32toa_countlut(0, buffer);
    EXPECT_EQ(1, shift3); // Сдвиг должен быть равен 1
    EXPECT_STREQ("0", buffer); // Буфер должен содержать "0\0"
    delete [] buffer;

    // NEGATIVE VALUE
    buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[11] = '\0';
    const int shift4 = i32toa_countlut(-1234567890, buffer);
    EXPECT_EQ(11, shift4); // Сдвиг должен быть равен 11
    EXPECT_STREQ("-1234567890", buffer); // Буфер должен содержать "-1234567890\0"
    delete [] buffer;

    // ZERO VALUE
    buffer = new char[15];
    memset(buffer, 0, 15);
    buffer[11] = '\0';
    const int shift5 = i32toa_countlut(-2147483648, buffer);
    EXPECT_EQ(11, shift5); // Сдвиг должен быть равен 11
    EXPECT_STREQ("-2147483648", buffer); // Буфер должен содержать "-2147483648\0"
    delete [] buffer;
}

TEST(CountLutsTestSuit, u64toaCountulTest) {
    // NULLPTR
    EXPECT_EXIT({int expect = u64toa_countlut(5, nullptr); // Ожидаем получить сдвиг равный 0
                 if (expect == 0) {
                     fprintf(stderr, "PASSED nullptr");
                 }
                 exit(0);}, ::testing::ExitedWithCode(0), "PASSED nullptr");

    // MAX VALUE
    char *buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[19] = '\0';
    const int shift = u64toa_countlut(9223372036854775807, buffer);
    EXPECT_EQ(19, shift); // Сдвиг должен быть равен 19
    EXPECT_STREQ("9223372036854775807", buffer); // Буфер должен содержать "9223372036854775807\0"
    delete [] buffer;

    // POSITIVE VALUE
    buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[19] = '\0';
    const int shift2 = u64toa_countlut(1234567890123456789, buffer);
    EXPECT_EQ(19, shift2); // Сдвиг должен быть равен 19
    EXPECT_STREQ("1234567890123456789", buffer); // Буфер должен содержать "1234567890123456789\0"
    delete [] buffer;

    // MIN VALUE
    buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[1] = '\0';
    const int shift3 = u64toa_countlut(0, buffer);
    EXPECT_EQ(1, shift3); // Сдвиг должен быть равен 1
    EXPECT_STREQ("0", buffer); // Буфер должен содержать "0\0"
    delete [] buffer;
}

TEST(CountLutsTestSuit, i64toaCountulTest) {
    // NULLPTR
    EXPECT_EXIT({int expect = i64toa_countlut(5, nullptr); // Ожидаем получить сдвиг равный 0
                 if (expect == 0) {
                     fprintf(stderr, "PASSED nullptr");
                 }
                 exit(0);}, ::testing::ExitedWithCode(0), "PASSED nullptr");

    // MAX VALUE
    char *buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[19] = '\0';
    const int shift = i64toa_countlut(9223372036854775807, buffer);
    EXPECT_EQ(19, shift); // Сдвиг должен быть равен 19
    EXPECT_STREQ("9223372036854775807", buffer); // Буфер должен содержать "9223372036854775807\0"
    delete [] buffer;

    // POSITIVE VALUE
    buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[19] = '\0';
    const int shift2 = i64toa_countlut(1234567890123456789, buffer);
    EXPECT_EQ(19, shift2); // Сдвиг должен быть равен 19
    EXPECT_STREQ("1234567890123456789", buffer); // Буфер должен содержать "1234567890123456789\0"
    delete [] buffer;

    // ZERO VALUE
    buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[1] = '\0';
    const int shift3 = i64toa_countlut(0, buffer);
    EXPECT_EQ(1, shift3); // Сдвиг должен быть равен 1
    EXPECT_STREQ("0", buffer); // Буфер должен содержать "0\0"
    delete [] buffer;

    // NEGATIVE VALUE
    buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[20] = '\0';
    const int shift4 = i64toa_countlut(-1234567890123456789, buffer);
    EXPECT_EQ(20, shift4); // Сдвиг должен быть равен 20
    EXPECT_STREQ("-1234567890123456789", buffer); // Буфер должен содержать "-1234567890123456789\0"
    delete [] buffer;

    // ZERO VALUE
    buffer = new char[25];
    memset(buffer, 0, 25);
    buffer[20] = '\0';
    const int shift5 = i64toa_countlut(-9223372036854775808, buffer);
    EXPECT_EQ(20, shift5); // Сдвиг должен быть равен 11
    EXPECT_STREQ("-9223372036854775808", buffer); // Буфер должен содержать "-9223372036854775808\0"
    delete [] buffer;
}
