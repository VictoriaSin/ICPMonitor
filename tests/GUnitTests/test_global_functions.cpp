#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "global_functions.h"

// Не учитывает, что функция crc8 получит размер больше,
// чем находится элементов в arr
TEST(GlobalFunctionsTestSuit, crc8Test) {
    // NULLPTR
    EXPECT_EXIT({int expect = crc8(nullptr, 30, 0); // Ожидаем получить 0
                 if (expect == 0x0) {
                     fprintf(stderr, "PASSED nullptr");
                 }
                 exit(0);}, ::testing::ExitedWithCode(0), "PASSED nullptr");

    // ZERO
    const char *arr = "";
    uint8_t expected = 0x0;
    const uint8_t received = crc8(arr, 0, 0);
    EXPECT_EQ(expected, received);

    // VALUE
    const char *arr1 = "pochemy eto ne rabotalo blin a";
    expected = 0x06;
    const uint8_t received1 = crc8(arr1, 30, 0);
    EXPECT_EQ(expected, received1);
}

TEST(GlobalFunctionsTestSuit, parseNumber) {
    const QString checkNums {"В 2023 году в году было 365 дней и 24 часа в сутках. По оценкам, население города А составляло около 1,5 миллиона человек. Температура в летние месяцы достигала палящих 40 градусов по Цельсию. человек потреблял около 2 литров воды в день.Ограничение скорости на шоссе было установлено на уровне 100 километров в час."
                             "На финансовом рынке цена акций компании X увеличилась на 10%, а цена акций компании Y снизилась на 5%. Уровень инфляции за год составил 2,5%. Темпы роста ВВП прогнозировались на уровне 3,2% в текущем финансовом году. Государственный долг достиг ошеломляющих 25 триллионов долларов."
                             "Во время футбольного матча окончательный счет был 3-2 в пользу хозяев поля. Нападающий оформил хет-трик, забив 3 гола из 5 попыток. Номер на футболке игрока был 7. Судья показал 4 желтые карточки и 1 красную карточку во время игры."};

    // Для проверки
    QVector<int64_t> numbersInText {2023, 365, 24, 1, 5, 40, 2, 100, 10, 5, 2, 5, 3, 2, 25, 3, 2, 3, 5, 7, 4, 1};

    // Парсим все целые числа из текста
    QVector<int64_t> parsedNumbers = parseIntegerNumbers(checkNums);

    EXPECT_TRUE(numbersInText == parsedNumbers);
}
