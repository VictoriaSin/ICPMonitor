#ifndef IDATASERIALIZER_H
#define IDATASERIALIZER_H

#include <memory>
#include <stdint.h>

class Label;

class IDataSerializer
{
public:
    /*! Сериализация данных
        Возвращает размер сериализованных данных в байтах
     */
    virtual int serializeData(char *dest) const = 0;

    /*! Десериализация данных
        Возвращает размер десериализованных данных в байтах
     */
    virtual int deserializeData(const char *dest) = 0;

    /*! Абсолютное время */
    virtual int64_t getAbsoluteTime() const = 0;

    /*! Возвращает максимальное значение,
        прочитаных и записанных значений
        в байтах.
     */
    virtual int getMaxReadOrWriteData() const = 0;

    virtual void setAbsoluteTime(int64_t) = 0;

    /*! Установка принадлежности к сессии */
    virtual void setSesionID(int) = 0;

    /*! Возврат id сессии, к которой принадлежит метка */
    virtual int getSesionID() const = 0;

    /*! Сбрасывает все установленные данные */
    virtual void reset() = 0;

    virtual ~IDataSerializer() = default;
};

#endif // IDATASERIALIZER_H
