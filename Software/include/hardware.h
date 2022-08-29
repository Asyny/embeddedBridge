#include <Arduino.h>

// datasheets
//MCP45HV31 https://eu.mouser.com/datasheet/2/268/20005304A-347268.pdf

// Serial
#define SERIAL_BAUD 115200
// UART
#define UART0_BAUD 115200
#define UART0_TX 0
#define UART0_RX 1
#define UART1_BAUD 115200
#define UART1_TX 8
#define UART1_RX 9

// SPI
#define SPI0_SCK 18
#define SPI0_MOSI 19
#define SPI0_MISO 16
// SPI CS
#define AD9833_DDS_CS 22
#define MCP2515_CAN1_CS 15
#define MCP2515_CAN2_CS 14
#define SD_CS 7

// GPIOs
#define LED_PIN 25
#define RELAIS1_PIN 20
#define RELAIS2_PIN 21
#define LEVELSHIFT1_PIN 10
#define LEVELSHIFT2_PIN 11
#define LEVELSHIFT3_PIN 12
#define LEVELSHIFT4_PIN 13
#define AMP_HL 3
#define SD_CD 17

// I2C
#define I2C0_SDA 4
#define I2C0_SCL 5
#define I2C1_SDA 26
#define I2C1_SCL 27
// I2C Adresses
#define INA219_VCC_ADDR 0x40
#define INA219_VDC_ADDR 0x41
#define INA219_DOUT1_ADDR 0x42
#define INA219_DOUT2_ADDR 0x43
#define INA219_BAT_ADDR 0x4A
#define MCP23017_DOUT_ADDR 0x20	
//#define MCP4728_AOUT_ADDR 0x60	// ToDo: BUG same adress as MCP4725 Battery
#define MCP4725_BAT_ADDR 0x60 // AJxx=110000n, APxx=110001n, AQxx=110010n, ARxx=110011n
#define MCP4725_DDSOFFSET_ADDR 0x61
#define ADS1115_AIN_ADDR 0x48
#define EEPROM_ADDR 0x50
#define MCP45HV31_VCC_ADDR 0x3C
#define MCP45HV31_VDC_ADDR 0x3D	// ToDo: BUG same address as MCP45HV31_VCC_ADDR

// MCP23017 GPIOs
#define VCC_EN_PIN 0
#define VCC_DOUT1_PIN 1
#define VCC_DOUT2_PIN 2
#define VCC_DOUT3_PIN 3
#define VCC_DOUT4_PIN 4
//#define UNUSED 5
//#define UNUSED 6
//#define UNUSED 7
#define VDC_EN_PIN 8
#define VDC_DOUT1_PIN 9
#define VDC_DOUT2_PIN 10
#define VDC_DOUT3_PIN 11
#define VDC_DOUT4_PIN 12
//#define UNUSED 13
//#define UNUSED 14
//#define UNUSED 15

// Settings
#define VCC_VDC_MAX_VOLTAGE 27.0

#define MCP23017_MAX_PIN 15

#define AIN1_VOLTAGE_DEVIDER 9.1266
#define AIN2_VOLTAGE_DEVIDER 8
#define AIN3_VOLTAGE_DEVIDER 8
#define AIN4_VOLTAGE_DEVIDER 8

#define FOUT_OFFSET_MULTIPLIER 160
#define BAT_MULTIPLIER 790
#define VCC_VOLTAGE_MULTIPLIER 4.561 // 1.25*(1+100k/4k7)=27.846V; max wiper (100k) = 127 -> 27.846 = 127, 0.219 = 1
#define VDC_VOLTAGE_MULTIPLIER 4.561

#define MENU_SERIAL Serial1
