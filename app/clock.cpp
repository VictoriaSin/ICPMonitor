#include "clock.h"
#include "global_functions.h"

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
    if (read(file,bufget,DEV_REGISTERS_NUM) != DEV_REGISTERS_NUM)
    {
        qDebug("Failed to read from the i2c bus");
        return false;
    }
    char strOut[100];
    sprintf(strOut, "Date/Time 20%02x-%02x-%02x %02x:%02x:00", bufget[5], bufget[4], bufget[3], bufget[1], bufget[0]);//, buf[1]);
    qDebug() << "Read date" << strOut;

    qDebug() << "str1"<< bufget[6]<< bufget[5]<< bufget[4]<< bufget[3] << bufget[2]<< bufget[1]<< bufget[0];
    close(file);
    return true;
}
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
