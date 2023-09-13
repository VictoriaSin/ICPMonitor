#ifndef CLOCK_H
#define CLOCK_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/gpio.h>
#include "owi_protocol.h"

enum I2C_RESULT
{
  I2C_OK,
  I2C_ERROR_OPEN,
  I2C_READ_FAILED_TO_IOCTL,
  I2C_WRITE_FAILED_TO_IOCTL,
};

uint8_t getRTC(u8 *data);
int8_t setRTC(u8 *data);

#endif // CLOCK_H
