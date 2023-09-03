#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <stdint.h>

#include <QList>

#include "global_enums.h"


/*! Стандартные пути к файлам Linux */
namespace LinuxPaths {
/*! Путь к файлу с информацией о RAM в Linux */
static constexpr char RAMInfoPathLinux[] {"/proc/meminfo"};
}

/*! Консольные команды Linux */
namespace LinuxCommands {
static constexpr char AwkCommandName[] {"awk"};
static constexpr char BashCommandName[] {"bash"};
}

/*! Генерация контрольной суммы
    size обязательно должен соответствовать
    реальному размеру arr - иначе UB
    Polynomial Formula(HEX) - 0x07
    (x^8) + x^2 + x + 1 - полином
*/
uint8_t crc8(const char *arr, uint32_t size, uint8_t crc = 0);

/*! Проверка на корректность файла */
bool isGoodFile(const QString &filePath);

/*! Перемещает указанный файл по указанному пути
    currentFilePath - текущий путь к файлу (Пример: /home/picture.png)
    targetPath - место, в которое перемещаем (Пример: /photo)
 */
bool moveFile(const QString &currentFilePath, const QString &targetPath);

/*! Создание папки, если она не существует
    Если папка есть или была успешно создана,
    то возвращается true. Иначе false.
    path - путь к новой папке
*/
bool makeDir(const QString &path);

/*! Исполняет консольную команду и возвращает её ответ.
    nameProgram - имя выполняемой программы
    arguments - аргументы программы
 */
QString executeAConsoleCommand(const QString &nameProgram, const QStringList &arguments);

/*! Парсит все целые числа из строки */
QVector<int64_t> parseIntegerNumbers(const QString &value);

/*! Возвращает общее кол-во оперативной памяти в байтах  (-1 при неудаче) */
int64_t totalRAMBytesLinux();

/*! Возвращает свободное кол-во оперативной памяти в байтах (-1 при неудаче) */
int64_t freeRAMBytesLinux();

/*! Обратный список QList */
template <typename T>
QList<T> reversed(const QList<T> & in) {
    QList<T> result;
    result.reserve(in.size());
    std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
    return result;
}

#endif // GLOBAL_FUNCTIONS_H
