#include <MD_AD9833.h>
#include <Adafruit_EEPROM_I2C.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_INA219.h>
#include <Adafruit_MCP4725.h>
#include <MCP45HVX1.h>


/*
 * TODO Hardware support:
 *
 * INA219 power supply monitoring
 * EEPROM
 * SD memory
 * ESP8266 (WiFi)
 * CAN
 * VDC (power rail 2) - no digital poti assembled
 * Calibration for AIN
 * RS232 forwarding to USB?
 * Analog out
 * FreqOut algorithm for setting frequency
 * ADC via DMA + interrupt?
 * Dicussion point: separate enable/disable function or common set... function with paramter?
 * Dicussion point: use either int or int16_t etc.?
*/
class HAL {

public:
    const static MD_AD9833::mode_t FreqOutModes[];

    enum DigOutMode
    {
        OFF = 0,
        VCC_ON = 1,
        VDC_ON = 2
    };

    enum PsuType
    {
        VCC_RAIL = 0,
        VDC_RAIL = 1
    };

    struct FreqOutSettings {
        int32_t frequency = 1000;
        float_t offset = 0.0;
        uint8_t mode = 0;
        uint8_t multiplier = 0;
    } FreqOut;

    // Singletons should not be cloneable
    HAL(HAL &rhs) = delete;
    // Singletons should not be assignable
    void operator=(const HAL&) = delete;

    static HAL* getInstance(void);

    void initialize(void);

    void setRelais1(bool status);
    void setRelais2(bool status);

    void setLevelShift1(bool status);
    void setLevelShift2(bool status);
    void setLevelShift3(bool status);
    void setLevelShift4(bool status);

    void setBatSimVoltage(float_t value);

    void setAinGain(adsGain_t gain);
    int16_t readAdcChannel(uint32_t channel, float_t& resultInVolts);

    void setFreqOutOffset(float_t offset);
    MD_AD9833::mode_t getFreqOutModeMapping(uint32_t mode) const;
    void setFreqOutMode(MD_AD9833::mode_t mode);
    void setFreqOutFrequency(uint32_t frequency, uint32_t multiplier);

    void configurePowerSupply(enum PsuType psuType, float_t voltage);
    void setVccOutput(bool enable);
    void setVdcOutput(bool enable);
    void setDigitalOut(uint32_t outputChannel, enum DigOutMode mode);

protected:
    static HAL* halInstance_;

    HAL(void);

private:
    MD_AD9833 ad;
    Adafruit_EEPROM_I2C i2ceeprom;
    Adafruit_ADS1115 ads;
    Adafruit_MCP23X17 mcp;
    Adafruit_INA219 ina_vcc;
    //Adafruit_INA219 ina_vdc;
    //Adafruit_INA219 ina_dout1;
    //Adafruit_INA219 ina_dout2;
    MCP45HVX1 mcp_vcc;
    // MCP45HVX1 mcp_vdc;
    Adafruit_MCP4725 dac_dds;
    Adafruit_MCP4725 dac_bat;

    void initializeSerials(void);
    void initializeDebugInterface(void);
    void initializeGpio(void);
    void initializeSpi(void);
    void initializeI2c(void);
    void initializePsu(void);
};