#ifndef CLOCK_H
#define CLOCK_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "stdint.h"
//#include <linux/gpio.h>
//#include "owi_protocol.h"

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t

enum I2C_RESULT
{
  I2C_OK,
  I2C_ERROR_OPEN,
  I2C_READ_FAILED_TO_IOCTL,
  I2C_WRITE_FAILED_TO_IOCTL,
};

uint8_t getRTC(u8 *data);
int8_t setRTC(u8 *data);
void setDateTime(u8 *data);

#endif // CLOCK_H
