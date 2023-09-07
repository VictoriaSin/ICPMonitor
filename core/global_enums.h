#ifndef GLOBAL_ENUMS_H
#define GLOBAL_ENUMS_H

/*! Ключи для аргументов глобальных событий */
namespace GlobalEventsArgs {
    constexpr static char Message[] = "MSG_GLOBAL";
}

/*! Доступные форматы конвертирования скриншота из PPM */
enum ConversionPictureFormat {
    JPEG = 0,
    PNG = 1,
    NONE = 2 // Без конвертирования
};

#endif // GLOBAL_ENUMS_H
