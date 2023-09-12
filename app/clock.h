#ifndef CLOCK_H
#define CLOCK_H

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/gpio.h>
#include "owi_protocol.h"

bool getRTC(u8 *bufget);
bool setRTC(u8 *buf);

#endif // CLOCK_H
