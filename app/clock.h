#ifndef CLOCK_H
#define CLOCK_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/gpio.h>
#include "owi_protocol.h"

bool getRTC(u8 *bufget);
int8_t setRTC(u8 *data);

#endif // CLOCK_H
