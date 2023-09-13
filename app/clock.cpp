#include "clock.h"
#include "global_functions.h"
#include <linux/i2c.h>
#include <linux/i2o-dev.h>

#define DEV_ADDRESS 0x68
#define DEV_REGISTERS_NUM 7
const char *devName = "/dev/i2c-2";
bool getRTC(u8 *bufget)
{
    int file = open(devName, O_RDWR); // Open up the I2C bus
    if (file == -1)
    {
        qDebug() << "Error open " << devName;
        return false;
    }

    u8 data[10];
    u8 reg_addr = 0;
    struct i2c_msg messages[] = {
          { DEV_ADDRESS, 0, 1, &reg_addr },         /*Set register address. */
          { DEV_ADDRESS, I2C_M_RD, 7, data },  /*Read length bytes into data. */
      };
      struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };


    if (ioctl(file, I2C_RDWR, &ioctl_data) != 2)
    {
        fprintf(stderr, "i2c_read: failed to ioctl: %s\n", strerror(errno));
        close(file);

    }
    char strOut[100];
    sprintf(strOut, "reg 0x00 = 0x%02x\r\nreg 0x01 = 0x%02x\r\nreg 0x02 = 0x%02x\r\nreg 0x03 = 0x%02x\r\nreg 0x04 = 0x%02x\r\nreg 0x05 = 0x%02x\r\nreg 0x06 = 0x%02x",
            ioctl_data.msgs[1].buf[0],
            ioctl_data.msgs[1].buf[1],
            ioctl_data.msgs[1].buf[2],
            ioctl_data.msgs[1].buf[3],
            ioctl_data.msgs[1].buf[4],
            ioctl_data.msgs[1].buf[5],
            ioctl_data.msgs[1].buf[6]);
    qDebug() << strOut;

     return -1;


     ///////////////////////


     // Specify the address of the slave device.
     if (ioctl(file, I2C_SLAVE, DEV_ADDRESS) < 0)
     {
         qDebug("Failed to acquire bus access and/or talk to slave");
         return false;
     }


    // Write a byte to the slave.
    char send[1];
    send[0] = 0;
    if (write(file, send, 1) != 1)
    {
        qDebug("Failed to write to the i2c bus");
        return false;
    }

    // Read a byte from the slave.
    if (read(file, bufget, DEV_REGISTERS_NUM) != DEV_REGISTERS_NUM)
    {
        qDebug("Failed to read from the i2c bus");
        return false;
    }

    sprintf(strOut, "Date/Time 20%02x-%02x-%02x %02x:%02x:00", bufget[5], bufget[4], bufget[3], bufget[1], bufget[0]);//, buf[1]);
    qDebug() << "Read date" << strOut;
    sprintf(strOut, "0x%02x - 0x%02x - 0x%02x - 0x%02x - 0x%02x - 0x%02x - 0x%02x",  bufget[6], bufget[5], bufget[4], bufget[3] , bufget[2], bufget[1], bufget[0]);
    qDebug() << "Read date 2" << strOut;


    //qDebug() << "str1"<< bufget[6]<< bufget[5]<< bufget[4]<< bufget[3] << bufget[2]<< bufget[1]<< bufget[0];
    close(file);
    return true;
}


int8_t setRTC(uint8_t *data)
{
  uint16_t length = 7;
  /*Open the I2C device. */
  int32_t fd = open(devName, O_RDWR);
  if (fd == -1)
  {
    fprintf(stderr, "i2c_write: failed to open: %s\n", strerror(errno));
    return -1;
  }

  /* I2C-Prepare a buffer for Write. */
  uint8_t buffer[8];
  buffer[0] = 0x00;              /*Set the register address in the 1st byte. */
  memcpy(&buffer[1], data, length);  /*Set data after the second byte. */

  /* I2C-Create a Write message. */
  struct i2c_msg message = { DEV_ADDRESS, 0, uint16_t(length + 1), buffer };
  struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };

  /* I2C-Write. */
  if (ioctl(fd, I2C_RDWR, &ioctl_data) != 1)
  {
    fprintf(stderr, "i2c_write: failed to ioctl: %s\n", strerror(errno));
    close(fd);
    return -1;
  }
  close(fd);
  char ttt[100];
  sprintf(ttt, "20%02x%02x%02x %02x:%02x:%02x", data[6], data[5], data[4], data[2], data[1], data[0]);
  //qDebug() << "set date = " <<ttt;
  QString res = executeAConsoleCommand("date", QStringList() << QString("--set=") + ttt);
  qDebug() << "Result " << res;
  return 0;
}
/*
bool setRTC(u8 *buf)
{
    const char *devName = "/dev/i2c-2";
    // Open up the I2C bus
    int file = open(devName, O_RDWR);
    if (file == -1)
    {
        qDebug() << "Error open " << devName;
        return false;
    }
    // Specify the address of the slave device.
    if (ioctl(file, I2C_SLAVE, DEV_ADDRESS) < 0)
    {
        qDebug("Failed to acquire bus access and/or talk to slave");
        return false;
    }

    char send[1];
    send[0] = 1;
    if (write(file, send, 1) != 1)
    {
        qDebug("Failed to write to the i2c bus");
        return false;
    }

    if (write(file, buf, DEV_REGISTERS_NUM) != DEV_REGISTERS_NUM)
    {
        qDebug("Failed to write to the i2c bus");
        return false;
    }
    char ttt[100];
    sprintf(ttt, "20%02x%02x%02x %02x:%02x:%02x", buf[6], buf[5], buf[4], buf[2], buf[1], buf[0]);
    qDebug() << "set date = " <<ttt;
    QString res = executeAConsoleCommand("date", QStringList() << QString("--set=") + ttt);
    qDebug() << "Result " << res;
    close(file);
    return true;
}
*/
