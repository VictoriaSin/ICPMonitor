#ifndef ZSC_H
#define ZSC_H
#include "stdint.h"
#include "../linux/spi/spidev.h"
#include <stdio.h>
#include <string>
#include "../app/global_define.h"

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t

#define SLAVE_ADDR_W 0xF0
#define SLAVE_ADDR_R 0xF1

#define GPIO_PIN_PATH_RESET "/sys/class/gpio/gpio25" // RESET
#define GPIO_PIN_PATH_CS "/sys/class/gpio/gpio26"   // OWI

#define POLYNOM 0xA005

#define CS_L()\
{ fpCS=fopen(GPIO_PIN_PATH_CS"/value","w");\
  fprintf(fpCS,"0");\
  fclose(fpCS); }

#define CS_H()\
{ fpCS=fopen(GPIO_PIN_PATH_CS"/value","w");\
  fprintf(fpCS,"1");\
  fclose(fpCS); }

#define CNT_REGS_CLC_CRC 0x1D   // Количество регистров, для которых нужно считать CRC
#define MAX_REGS 32             // Всего регистров

#define ZSC_RAM 0x10
#define ZSC_EEP 0x30

#define START_CYC_EEP 0x01       // Start measurement cycle including initialization from EEPROM (350μs)
#define START_CYC_RAM 0x02       // Start measurement cycle including initialization from RAM (220μs)

#define START_OM      0x70       // Start Open Mode Note: In Open Mode the command set is restricted! Change to Command Mode is possible
#define START_NOM     0x71       // Start Normal Operation Mode (NOM) In Normal Operation Mode, the command set is restricted! Change to Command Mode or Open Mode is NOT possible
#define START_CM      0x72       // Start Command Mode (CM) Complete command set! Change to Open Mode or Normal Operation Mode is possible

#define ZSC_CMD_00h_R_RAM  0x10 // (50μs)
#define ZSC_CMD_01h_R_RAM  0x11 // (50μs)
#define ZSC_CMD_02h_R_RAM  0x12 // (50μs)
#define ZSC_CMD_03h_R_RAM  0x13 // (50μs)
#define ZSC_CMD_04h_R_RAM  0x14 // (50μs)
#define ZSC_CMD_05h_R_RAM  0x15 // (50μs)
#define ZSC_CMD_06h_R_RAM  0x16 // (50μs)
#define ZSC_CMD_07h_R_RAM  0x17 // (50μs)
#define ZSC_CMD_08h_R_RAM  0x18 // (50μs)
#define ZSC_CMD_09h_R_RAM  0x19 // (50μs)
#define ZSC_CMD_0Ah_R_RAM  0x1A // (50μs)
#define ZSC_CMD_0Bh_R_RAM  0x1B // (50μs)
#define ZSC_CMD_0Ch_R_RAM  0x1C // (50μs)
#define ZSC_CMD_0Dh_R_RAM  0x1D // (50μs)
#define ZSC_CMD_0Eh_R_RAM  0x1E // (50μs)
#define ZSC_CMD_0Fh_R_RAM  0x1F // (50μs)

#define ZSC_CMD_10h_R_RAM  0x20 // (50μs)
#define ZSC_CMD_11h_R_RAM  0x21 // (50μs)
#define ZSC_CMD_12h_R_RAM  0x22 // (50μs)
#define ZSC_CMD_13h_R_RAM  0x23 // (50μs)
#define ZSC_CMD_14h_R_RAM  0x24 // (50μs)
#define ZSC_CMD_15h_R_RAM  0x25 // (50μs)
#define ZSC_CMD_16h_R_RAM  0x26 // (50μs)
#define ZSC_CMD_17h_R_RAM  0x27 // (50μs)
#define ZSC_CMD_18h_R_RAM  0x28 // (50μs)
#define ZSC_CMD_19h_R_RAM  0x29 // (50μs)
#define ZSC_CMD_1Ah_R_RAM  0x2A // (50μs)
#define ZSC_CMD_1Bh_R_RAM  0x2B // (50μs)
#define ZSC_CMD_1Ch_R_RAM  0x2C // (50μs)
#define ZSC_CMD_1Dh_R_RAM  0x2D // (50μs)
#define ZSC_CMD_1Eh_R_RAM  0x2E // (50μs)  User free memory
#define ZSC_CMD_1Fh_R_RAM  0x2F // (50μs)  User free memory

#define ZSC_CMD_00h_R_EEP  0x30 // (50μs)
#define ZSC_CMD_01h_R_EEP  0x31 // (50μs)
#define ZSC_CMD_02h_R_EEP  0x32 // (50μs)
#define ZSC_CMD_03h_R_EEP  0x33 // (50μs)
#define ZSC_CMD_04h_R_EEP  0x34 // (50μs)
#define ZSC_CMD_05h_R_EEP  0x35 // (50μs)
#define ZSC_CMD_06h_R_EEP  0x36 // (50μs)
#define ZSC_CMD_07h_R_EEP  0x37 // (50μs)
#define ZSC_CMD_08h_R_EEP  0x38 // (50μs)
#define ZSC_CMD_09h_R_EEP  0x39 // (50μs)
#define ZSC_CMD_0Ah_R_EEP  0x3A // (50μs)
#define ZSC_CMD_0Bh_R_EEP  0x3B // (50μs)
#define ZSC_CMD_0Ch_R_EEP  0x3C // (50μs)
#define ZSC_CMD_0Dh_R_EEP  0x3D // (50μs)
#define ZSC_CMD_0Eh_R_EEP  0x3E // (50μs)
#define ZSC_CMD_0Fh_R_EEP  0x3F // (50μs)

#define ZSC_CMD_10h_R_EEP  0x40 // (50μs)
#define ZSC_CMD_11h_R_EEP  0x41 // (50μs)
#define ZSC_CMD_12h_R_EEP  0x42 // (50μs)
#define ZSC_CMD_13h_R_EEP  0x43 // (50μs)
#define ZSC_CMD_14h_R_EEP  0x44 // (50μs)
#define ZSC_CMD_15h_R_EEP  0x45 // (50μs)
#define ZSC_CMD_16h_R_EEP  0x46 // (50μs)
#define ZSC_CMD_17h_R_EEP  0x47 // (50μs)
#define ZSC_CMD_18h_R_EEP  0x48 // (50μs)
#define ZSC_CMD_19h_R_EEP  0x49 // (50μs)
#define ZSC_CMD_1Ah_R_EEP  0x4A // (50μs)
#define ZSC_CMD_1Bh_R_EEP  0x4B // (50μs)
#define ZSC_CMD_1Ch_R_EEP  0x4C // (50μs)
#define ZSC_CMD_1Dh_R_EEP  0x4D // (50μs)
#define ZSC_CMD_1Eh_R_EEP  0x4E // (50μs)  User free memory
#define ZSC_CMD_1Fh_R_EEP  0x4F // (50μs)  User free memory

#define ZSC_CMD_00h_W_RAM  0x80 // (50μs)
#define ZSC_CMD_01h_W_RAM  0x81 // (50μs)
#define ZSC_CMD_02h_W_RAM  0x82 // (50μs)
#define ZSC_CMD_03h_W_RAM  0x83 // (50μs)
#define ZSC_CMD_04h_W_RAM  0x84 // (50μs)
#define ZSC_CMD_05h_W_RAM  0x85 // (50μs)
#define ZSC_CMD_06h_W_RAM  0x86 // (50μs)
#define ZSC_CMD_07h_W_RAM  0x87 // (50μs)
#define ZSC_CMD_08h_W_RAM  0x88 // (50μs)
#define ZSC_CMD_09h_W_RAM  0x89 // (50μs)
#define ZSC_CMD_0Ah_W_RAM  0x8A // (50μs)
#define ZSC_CMD_0Bh_W_RAM  0x8B // (50μs)
#define ZSC_CMD_0Ch_W_RAM  0x8C // (50μs)
#define ZSC_CMD_0Dh_W_RAM  0x8D // (50μs)
#define ZSC_CMD_0Eh_W_RAM  0x8E // (50μs)
#define ZSC_CMD_0Fh_W_RAM  0x8F // (50μs)

#define ZSC_CMD_10h_W_RAM  0x90 // (50μs)
#define ZSC_CMD_11h_W_RAM  0x91 // (50μs)
#define ZSC_CMD_12h_W_RAM  0x92 // (50μs)
#define ZSC_CMD_13h_W_RAM  0x93 // (50μs)
#define ZSC_CMD_14h_W_RAM  0x94 // (50μs)
#define ZSC_CMD_15h_W_RAM  0x95 // (50μs)
#define ZSC_CMD_16h_W_RAM  0x96 // (50μs)
#define ZSC_CMD_17h_W_RAM  0x97 // (50μs)
#define ZSC_CMD_18h_W_RAM  0x98 // (50μs)
#define ZSC_CMD_19h_W_RAM  0x99 // (50μs)
#define ZSC_CMD_1Ah_W_RAM  0x9A // (50μs)
#define ZSC_CMD_1Bh_W_RAM  0x9B // (50μs)
#define ZSC_CMD_1Ch_W_RAM  0x9C // (50μs)
#define ZSC_CMD_1Dh_W_RAM  0x9D // (50μs)
#define ZSC_CMD_1Eh_W_RAM  0x9E // (50μs)  User free memory
#define ZSC_CMD_1Fh_W_RAM  0x9F // (50μs)  User free memory

#define ZSC_CMD_00h_W_EEP  0xA0 // (50μs)
#define ZSC_CMD_01h_W_EEP  0xA1 // (50μs)
#define ZSC_CMD_02h_W_EEP  0xA2 // (50μs)
#define ZSC_CMD_03h_W_EEP  0xA3 // (50μs)
#define ZSC_CMD_04h_W_EEP  0xA4 // (50μs)
#define ZSC_CMD_05h_W_EEP  0xA5 // (50μs)
#define ZSC_CMD_06h_W_EEP  0xA6 // (50μs)
#define ZSC_CMD_07h_W_EEP  0xA7 // (50μs)
#define ZSC_CMD_08h_W_EEP  0xA8 // (50μs)
#define ZSC_CMD_09h_W_EEP  0xA9 // (50μs)
#define ZSC_CMD_0Ah_W_EEP  0xAA // (50μs)
#define ZSC_CMD_0Bh_W_EEP  0xAB // (50μs)
#define ZSC_CMD_0Ch_W_EEP  0xAC // (50μs)
#define ZSC_CMD_0Dh_W_EEP  0xAD // (50μs)
#define ZSC_CMD_0Eh_W_EEP  0xAE // (50μs)
#define ZSC_CMD_0Fh_W_EEP  0xAF // (50μs)

#define ZSC_CMD_10h_W_EEP  0xB0 // (50μs)
#define ZSC_CMD_11h_W_EEP  0xB1 // (50μs)
#define ZSC_CMD_12h_W_EEP  0xB2 // (50μs)
#define ZSC_CMD_13h_W_EEP  0xB3 // (50μs)
#define ZSC_CMD_14h_W_EEP  0xB4 // (50μs)
#define ZSC_CMD_15h_W_EEP  0xB5 // (50μs)
#define ZSC_CMD_16h_W_EEP  0xB6 // (50μs)
#define ZSC_CMD_17h_W_EEP  0xB7 // (50μs)
#define ZSC_CMD_18h_W_EEP  0xB8 // (50μs)
#define ZSC_CMD_19h_W_EEP  0xB9 // (50μs)
#define ZSC_CMD_1Ah_W_EEP  0xBA // (50μs)
#define ZSC_CMD_1Bh_W_EEP  0xBB // (50μs)
#define ZSC_CMD_1Ch_W_EEP  0xBC // (50μs)
#define ZSC_CMD_1Dh_W_EEP  0xBD // (50μs)
#define ZSC_CMD_1Eh_W_EEP  0xBE // (50μs)  User free memory
#define ZSC_CMD_1Fh_W_EEP  0xBF // (50μs)  User free memory


// Registers
// Conditioning coefficients - Correction formula for measurand (see section 2.3)
#define ZSC_00h  0x00    // c0 - Offset
#define ZSC_01h  0x01    // c1 - Gain
#define ZSC_02h  0x02    // c2 - Non-linearity 2nd order
#define ZSC_03h  0x03    // c3 - Non-linearity 3rd order
#define ZSC_04h  0x04    // c4 - Temperature coefficient Bridge Offset 1st
#define ZSC_05h  0x05    // c5 - Temperature coefficient Bridge Offset 2nd
#define ZSC_06h  0x06    // c6 - Temperature coefficient Gain 1st order
#define ZSC_07h  0x07    // c7 - Temperature coefficient Gain 2nd order

// Conditioning coefficients – Limit the output at OUT pin (see section 2.4)
#define ZSC_08h  0x08    // lmin - Lower Limit with reference to pin OUT for voltage, current or PWM output
#define ZSC_09h  0x09    // lmax - Upper Limit with reference to pin OUT for voltage, current or PWM output

// Conditioning coefficients - Correction formula for temperature (see section 2.3)
#define ZSC_0Ah  0x0A    // t10 - Offset Temperature 1
#define ZSC_0Bh  0x0B    // t11 - Gain Temperature 1
#define ZSC_0Ch  0x0C    // t12 - Non-linearity 2nd order Temperature 1
#define ZSC_0Dh  0x0D    // t20 - Offset Temperature 2
#define ZSC_0Eh  0x0E    // t21 - Gain Temperature 2
#define ZSC_0Fh  0x0F    // t22 - Non-linearity 2nd order Temperature 2

// Conditioning coefficients – Limit the output at IO1 and IO2 pins (see section 2.4)
#define ZSC_10h  0x10    // Alarm 1: Threshold PWM1 : Lower Limit
#define ZSC_11h  0x11    // Alarm 1: Hysteresis PWM1 : Upper Limit
#define ZSC_12h  0x12    // Alarm 1: On-/Off-delay (two 8 bit values)
#define ZSC_13h  0x13    // Alarm 2: Threshold Common-mode voltage : Lower Limit
#define ZSC_14h  0x14    // Alarm 2: Hysteresis Common-mode voltage : Upper Limit
#define ZSC_15h  0x15    // Alarm 2: On-/Off-delay (two 8 bit values)

// Configuration words (see section 5.3)
#define ZSC_16h  0x16    // CFGCYC: Configuration of measurement cycle
#define ZSC_17h  0x17    // CFGSIF: Configuration of digital serial interface
#define ZSC_18h  0x18    // CFGAPP: Configuration of target application
#define ZSC_19h  0x19    // CFGAFE: Configuration of analog front end
#define ZSC_1Ah  0x1A    // CFGTMP: Configuration of temperature measurement
#define ZSC_1Bh  0x1B    // CFGOUT: Configuration of signal outputs
#define ZSC_1Ch  0x1C    // ADJREF: Adjustment of internal references

#define ZSC_1Dh  0x1D    // Signature
#define ZSC_1Eh  0x1E    // Free user memory, not included in Signature
#define ZSC_1Fh  0x1F    // Free user memory, not included in Signature

/* Default values */

// Conditioning coefficients - Correction formula for measurand (see section 2.3)
#define ZSC_00h_DEF  0x0800    // c0 - Offset
#define ZSC_01h_DEF  0x2000    // c1 - Gain
#define ZSC_02h_DEF  0x0000    // c2 - Non-linearity 2nd order
#define ZSC_03h_DEF  0x0000    // c3 - Non-linearity 3rd order
#define ZSC_04h_DEF  0x0000    // c4 - Temperature coefficient Bridge Offset 1st
#define ZSC_05h_DEF  0x0000    // c5 - Temperature coefficient Bridge Offset 2nd
#define ZSC_06h_DEF  0x0000    // c6 - Temperature coefficient Gain 1st order
#define ZSC_07h_DEF  0x0000    // c7 - Temperature coefficient Gain 2nd order

// Conditioning coefficients – Limit the output at OUT pin (see section 2.4)
#define ZSC_08h_DEF  0x0000    // lmin - Lower Limit with reference to pin OUT for voltage, current or PWM output
#define ZSC_09h_DEF  0x07FF    // lmax - Upper Limit with reference to pin OUT for voltage, current or PWM output

// Conditioning coefficients - Correction formula for temperature (see section 2.3)
#define ZSC_0Ah_DEF  0x1000    // t10 - Offset Temperature 1
#define ZSC_0Bh_DEF  0x2000    // t11 - Gain Temperature 1
#define ZSC_0Ch_DEF  0x0000    // t12 - Non-linearity 2nd order Temperature 1
#define ZSC_0Dh_DEF  0x1000    // t20 - Offset Temperature 2
#define ZSC_0Eh_DEF  0x2000    // t21 - Gain Temperature 2
#define ZSC_0Fh_DEF  0x0000    // t22 - Non-linearity 2nd order Temperature 2

// Conditioning coefficients – Limit the output at IO1 and IO2 pins (see section 2.4)
#define ZSC_10h_DEF  0x0000    // Alarm 1: Threshold PWM1 : Lower Limit
#define ZSC_11h_DEF  0x01FF    // Alarm 1: Hysteresis PWM1 : Upper Limit
#define ZSC_12h_DEF  0x0000    // Alarm 1: On-/Off-delay (two 8 bit values)
#define ZSC_13h_DEF  0x0000    // Alarm 2: Threshold Common-mode voltage : Lower Limit
#define ZSC_14h_DEF  0xFFFF    // Alarm 2: Hysteresis Common-mode voltage : Upper Limit
#define ZSC_15h_DEF  0x0000    // Alarm 2: On-/Off-delay (two 8 bit values)

// Configuration words (see section 5.3)
#define ZSC_16h_DEF  0x0048    // CFGCYC: Configuration of measurement cycle
#define ZSC_17h_DEF  0x0034    // CFGSIF: Configuration of digital serial interface
#define ZSC_18h_DEF  0x7625    // CFGAPP: Configuration of target application
#define ZSC_19h_DEF  0x9800    // CFGAFE: Configuration of analog front end
#define ZSC_1Ah_DEF  0x0124    // CFGTMP: Configuration of temperature measurement
#define ZSC_1Bh_DEF  0x8060    // CFGOUT: Configuration of signal outputs
#define ZSC_1Ch_DEF  0x9258    // ADJREF: Adjustment of internal references

#define ZSC_1Dh_DEF  0x159D    // Signature
#define ZSC_1Eh_DEF  0x0000    // Free user memory, not included in Signature
#define ZSC_1Fh_DEF  0x0000    // Free user memory, not included in Signature


/* reg 17h */

#define SIFMD_MASK    0xFFFE  // SIF mode Chose active communication mode: 0: I²CTM communication 1: SPI communication
#define SIFMD_USE_SPI 0x0001  // 1: SPI communication
#define SIFMD_USE_I2C 0x0000  // 0: I²CTM communication

/* reg 18h */

#define OSCF_MASK      0xFF9F   // Raw oscillator frequency adjust 00: fCLK = 1 MHz 10: fCLK = 2 MHz 01: fCLK = 2 MHz 11: fCLK = 4 MHz
#define OSCF_Fclk_1MHz 0x0000   // 00: fCLK = 1 MHz
#define OSCF_Fclk_2MHz 0x0020   // 01: fCLK = 2 MHz
#define OSCF_Fclk_4MHz 0x0060   // 11: fCLK = 4 MHz


class ZSC
{
  public:
    ZSC();
    void terminate();
    void test(u16 iter, u32 delay_ms);
    u16 data[2];
#ifndef PC_BUILD
#ifndef TEST_BUILD
    void spi_oneShot();
#endif
#endif
    void resetRegsValues();
    int mFd {-1};
protected:
    bool mIsOpen = false;
    std::string spi_name;
    struct SPISetting
    {
        uint32_t mode;
        uint32_t speed;
        uint8_t  bits;
        uint16_t delay;
        uint8_t  lsb;
    };
    struct SPISetting mSetting;
    u8 txBuffer[32];
    u8 rxBuffer[32];
    struct spi_ioc_transfer mMessageTX;
    struct spi_ioc_transfer mMessageRX;
    FILE *fpCS = nullptr;

    bool spi_open();
    bool spi_close();
    bool spi_readSPI();
    bool spi_write();
    bool SPI_CMD(u8 CMD);
    u16 spi_getReg(u8 reg, u8 memType);
    u16 signature(u16* eepcont, u16 N);
    void resetZSC();
    void initPins();
    void initZSC();
    bool spi_saveReg(u8 reg, u16 data, u8 memType);
    uint16_t xOffset;

    const u16 initRegs[32]
    {
      0x0100, 0x2000, 0x0000, 0x0000,
      0x0000, 0x0000, 0x0000, 0x0000,
      0x0000, 0x07FF, 0x1000, 0x2000,
      0x0000, 0x1000, 0x2000, 0x0000,
      0x0000, 0x1F00, 0x0000, 0x0000,
      0xFFFF, 0x0000, 0x0048, 0x0015,
      0x7629, 0x900C, 0x0124, 0x8060,
      0x15C9, 0xE2A2, 0x0000, 0x0001
    };
    u16 regsValue[32];

};

#endif // ZSC_H
