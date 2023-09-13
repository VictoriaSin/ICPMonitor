#include "clock.h"
#include "global_functions.h"
#include <linux/i2c.h>
#include <linux/i2o-dev.h>

#define DEV_ADDRESS 0x68
#define DEV_REGISTERS_NUM 7
const char *devName = "/dev/i2c-2";

uint8_t getRTC(u8 *data)
{
#define I2C_M_WR 0
  u8 reg_addr = 0x00;               // Read from reg 0x00
  int file;
  struct i2c_msg messages[] =
  {
    { DEV_ADDRESS, I2C_M_WR, 1, &reg_addr }, // Set register address.
    { DEV_ADDRESS, I2C_M_RD, 7, data },      // Read length bytes into data.
  };
  struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };

  if ((file = open(devName, O_RDWR)) == -1) { return I2C_RESULT::I2C_ERROR_OPEN; }

  if (ioctl(file, I2C_RDWR, &ioctl_data) != 2)
  {
    close(file);
    return I2C_RESULT::I2C_READ_FAILED_TO_IOCTL;
  }
  return I2C_RESULT::I2C_OK;
}


int8_t setRTC(uint8_t *data)
{
  uint16_t length = 7;
  int file;
  uint8_t buffer[8];
  buffer[0] = 0x00;              /*Set the register address in the 1st byte. */

  if ((file = open(devName, O_RDWR)) == -1) { return I2C_RESULT::I2C_ERROR_OPEN; }

  /* I2C-Prepare a buffer for Write. */
  memcpy(&buffer[1], data, length);  /*Set data after the second byte. */

  /* I2C-Create a Write message. */
  struct i2c_msg message = { DEV_ADDRESS, 0, uint16_t(length + 1), buffer };
  struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };


  if (ioctl(file, I2C_RDWR, &ioctl_data) != 1)  // I2C-Write.
  {
    close(file);
    return I2C_RESULT::I2C_WRITE_FAILED_TO_IOCTL;
  }
  close(file);
  return I2C_RESULT::I2C_OK;
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

*/
