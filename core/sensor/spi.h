#ifndef SPI_H
#define SPI_H

#include <string>
#include <linux/spi/spidev.h>

class SPI
{
    /*! Настройки устройства SPI */
    struct SPISetting {
        uint32_t mode;
        uint32_t speed;
        uint8_t bits;
        uint16_t delay;
        uint8_t lsb;
    };

    //! Дескриптор файла, через который общаемся с устройством
    int mFd {-1};

    //! Настройки устройства SPI
    struct SPISetting mSetting {};

    //! Признак готовности к записи/чтению в/с SPI
    bool mIsOpen = false;

    //! Расположение устройства (путь к файлу)
    const std::string mDeviceName;

    //! Параметры отправки сообщения устройству, подключенному по SPI
    struct spi_ioc_transfer mMessage {};

public:
    explicit SPI(const std::string &_deviceName, uint32_t _mode, uint32_t _speed,
                 uint8_t _bits = 8, uint16_t _delay = 0, uint8_t _lsb = 0);
    /*! Задаём настройки общения с устройством SPI
        mode - принимает значение SPI_MODE_0 - SPI_MODE_3
        speed - скорость общения в Гц
     */
    void setSettingSpiDevice(uint32_t _mode, uint32_t _speed, uint8_t _bits = 8,
                             uint16_t _delay = 0, uint8_t _lsb = 0);
    /*! Возвращает признак готовности к чтению/записи с/в SPI*/
    bool isOpen();
    /*! Передача сообщения
     TX - указатель на отправляемый массив байтов
     RX - указатель на получаемый массив байтов
     len - кол-во байт, сколько ожидаем получить (обычно len + 1)
     Вывод функции: false - отправки не было или устройство не готово к чтению/записи; true - отправка была
     */
    bool transferSPI(uint8_t const *TX, uint8_t *RX, uint32_t len);
    /*! Чтение сообщений от устройства
     RX - указатель на получаемый массив байтов
     len - кол-во байт, сколько ожидаем получить (обычно len + 1)
     Вывод функции: false - отправки не было или устройство не готово к чтению/записи; true - отправка была
     */
    bool readSPI(uint8_t *RX, uint32_t len);
    /*! Запись в устройство
     TX - указатель на отправляемый массив байтов
     len - кол-во отправляемых байт
     Вывод функции: false - отправки не было или устройство не готово к чтению/записи; true - отправка была
     */
    bool writeSPI(uint8_t const *TX, uint32_t len);
    /*! Инициализация устройства
     Вывод функции: true - устройство инициализировано успешно, false - ошибка при инициализации устройства
     */
    bool open();
    /*! Деинициализация устройства
     Вывод функции: true - деинициализировано успешно, false - ошибка при деинициализации
     */
    bool close();
};

#endif // SPI_H
