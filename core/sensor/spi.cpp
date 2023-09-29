#include "spi.h"

#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

SPI::SPI(const std::string &_deviceName, uint32_t _mode, uint32_t _speed, uint8_t _bits, uint16_t _delay, uint8_t _lsb)
    : mDeviceName(_deviceName)
{
    mSetting.mode = _mode;
    mSetting.speed = _speed;
    mSetting.bits = _bits;
    mSetting.delay = _delay;
    mSetting.lsb = _lsb;

    memset(&mMessage, 0, sizeof(mMessage));
}

void SPI::setSettingSpiDevice(uint32_t _mode, uint32_t _speed, uint8_t _bits, uint16_t _delay, uint8_t _lsb)
{
    mSetting.mode = _mode;
    mSetting.speed = _speed;
    mSetting.bits = _bits;
    mSetting.delay = _delay;
    mSetting.lsb = _lsb;
}

bool SPI::isOpen()
{
    return mIsOpen;
}

bool SPI::open()
{
    if (isOpen()) {
        return true;
    }

    mFd = ::open(mDeviceName.c_str(), O_RDWR);
    // Если файл не найден
    if (mFd < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем режим чтения
    if (ioctl(mFd, SPI_IOC_WR_MODE, &mSetting.mode) < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем режим записи
    if (ioctl(mFd, SPI_IOC_RD_MODE, &mSetting.mode) < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем количество бит на слово при записи
    if (ioctl(mFd, SPI_IOC_WR_BITS_PER_WORD, &mSetting.bits) < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем количество бит на слово при чтении
    if (ioctl(mFd, SPI_IOC_RD_BITS_PER_WORD, &mSetting.bits) < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем битовое выравнивание для чтения
    if (ioctl(mFd, SPI_IOC_RD_LSB_FIRST, &mSetting.bits) < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем битовое выравнивание для записи
    if (ioctl(mFd, SPI_IOC_WR_LSB_FIRST, &mSetting.bits) < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем максимальную скорость передачи данных по SPI в Гц
    if (ioctl(mFd, SPI_IOC_WR_MAX_SPEED_HZ, &mSetting.speed) < 0) {
        mIsOpen = false;
        return false;
    }

    // Устанавливаем максимальную скорость чтения данных по SPI в Гц
    if (ioctl(mFd, SPI_IOC_RD_MAX_SPEED_HZ, &mSetting.speed) < 0) {
        mIsOpen = false;
        return false;
    }
    mIsOpen = true;
    return true;
}

bool SPI::close()
{
    if(::close(mFd) != 0) {
        mFd = -1;
        return false;
    }
    mIsOpen = false;
    return true;
}

bool SPI::transferSPI(uint8_t const *TX, uint8_t *RX, uint32_t len)
{
    // Если устройство не готово к чтению и записи
    if (!mIsOpen) { return false; }

    mMessage =
    {
        .tx_buf = (unsigned long)TX,
        .rx_buf = (unsigned long)RX,
        .len = len,
        .speed_hz = mSetting.speed,
        .delay_usecs = mSetting.delay,
        .bits_per_word = mSetting.bits,
        .cs_change = 0,
    };

    // Передаём сообщение
    if (ioctl(mFd, SPI_IOC_MESSAGE(1), &mMessage) < 1)
    {
        return false;
    }
    return true;
}

bool SPI::readSPI(uint8_t *RX, uint32_t len)
{
    return transferSPI(NULL, RX, len);
}

bool SPI::writeSPI(uint8_t const *TX, uint32_t len)
{
    return transferSPI(TX, NULL, len);
}
