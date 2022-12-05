#include "HAL.h"

#include <Arduino_DebugUtils.h>
#include <SPI.h>
#include <Wire.h>

// custom libraries
#include "hardware.h"
#include "MultiMap.h"

#define DEBUG_SERIAL    Serial1
#define DEBUG_LEVEL     DBG_VERBOSE	// DBG_NONE, DBG_ERROR, DBG_WARNING, DBG_INFO, DBG_DEBUG, DBG_VERBOSE


HAL* HAL::halInstance_= nullptr;

const MD_AD9833::mode_t HAL::FreqOutModes[] = {
	MD_AD9833::MODE_OFF,
	MD_AD9833::MODE_SINE,
	MD_AD9833::MODE_SQUARE1,
	MD_AD9833::MODE_SQUARE2,
	MD_AD9833::MODE_TRIANGLE 
};


HAL::HAL() : 
ad(SPI0_MOSI, SPI0_SCK, AD9833_DDS_CS),
i2ceeprom(),
ads(),
mcp(),
ina_vcc(INA219_VCC_ADDR),
// ina_vdc(INA219_VDC_ADDR),
// ina_dout1(INA219_DOUT1_ADDR),
// ina_dout2(INA219_DOUT2_ADDR),
mcp_vcc(MCP45HV31_VCC_ADDR),
// mcp_vdc(MCP45HV31_VDC_ADDR),
dac_dds(),
dac_bat() {

}

HAL* HAL::getInstance() {
    // This is a safer way to create an instance. instance = new Singleton is
    // dangeruous in case two instance threads wants to access at the same time
    if (halInstance_ == nullptr) {
        halInstance_ = new HAL();
    }
    return halInstance_;
}


void HAL::initialize(void) {
    initializeSerials();
    initializeDebugInterface();
    initializeGpio();
    initializeSpi();
    initializeI2c();
    initializePsu();
}


void HAL::setRelais1(bool status) {
    digitalWrite(RELAIS1_PIN, status);
}


void HAL::setRelais2(bool status) {
    digitalWrite(RELAIS2_PIN, status);
}


void HAL::setLevelShift1(bool status)
{
	digitalWrite(LEVELSHIFT1_PIN, status);
}


void HAL::setLevelShift2(bool status)
{
	digitalWrite(LEVELSHIFT2_PIN, status);
}


void HAL::setLevelShift3(bool status)
{
	digitalWrite(LEVELSHIFT3_PIN, status);
}


void HAL::setLevelShift4(bool status)
{
	digitalWrite(LEVELSHIFT4_PIN, status);
}


void HAL::setBatSimVoltage(float_t value)
{
	uint16_t bat_out = static_cast<uint16_t>(value * BAT_MULTIPLIER);
	bat_out = min(bat_out,4095);
	dac_bat.setVoltage(bat_out, false);
}


void HAL::setAinGain(adsGain_t gain)
{
	ads.setGain(gain);
}


int16_t HAL::readAdcChannel(uint32_t channel, float_t& resultInVolts)
{
	int16_t counts = ads.readADC_SingleEnded(channel);

	float_t convertedAdcResult = ads.computeVolts(counts);
	switch (channel)
	{
		case 0:
			resultInVolts = convertedAdcResult * AIN1_VOLTAGE_DEVIDER;
			break;

		case 1:
			resultInVolts = convertedAdcResult * AIN2_VOLTAGE_DEVIDER;
			break;

		case 2:
			resultInVolts = convertedAdcResult * AIN3_VOLTAGE_DEVIDER;
			break;

		case 3:
			resultInVolts = convertedAdcResult * AIN4_VOLTAGE_DEVIDER;
			break;

		default:
			resultInVolts = 0;
			DEBUG_SERIAL.print("Convertion error ");
			break;
	}

	return counts;
}


void HAL::setFreqOutOffset(float_t offset)
{
	uint16_t offsetInt = static_cast<uint16_t>(offset* FOUT_OFFSET_MULTIPLIER);
	dac_dds.setVoltage(offsetInt, false);
}


MD_AD9833::mode_t HAL::getFreqOutModeMapping(uint32_t mode) const
{
	uint32_t maxNofModes = sizeof(FreqOutModes) / sizeof( FreqOutModes[mode]);
	if (mode < maxNofModes)
		return FreqOutModes[mode];
	else {
		DEBUG_SERIAL.print("Unsupported frequency output mode");
		return MD_AD9833::MODE_OFF;
	}
}


void HAL::setFreqOutMode(MD_AD9833::mode_t mode)
{
	ad.setMode(mode);
}


void HAL::setFreqOutFrequency(uint32_t frequency, uint32_t multiplier)
{
	//TODO: computer multiplier via algo based on min/max values. user shall just set frequency to set it everything
	uint32_t frequencyToSet = frequency;
	
	if(multiplier > 0)
	{
		uint32_t multip = 1000;
		for(uint32_t i = 1; i < multiplier; i++) {
			multip *= multip;
		}
		frequencyToSet *= multip;
	}

	ad.setFrequency(MD_AD9833::CHAN_0, frequencyToSet);
}


void HAL::configurePowerSupply(enum PsuType psuType, float_t voltage)
{
	voltage = constrain(voltage, 0.0, VCC_VDC_MAX_VOLTAGE);

	float_t potiOutputValues[] = {12, 25, 38, 50, 63, 76, 89, 102, 114, 127};
	float_t voltageOutputValues[] = {4.87, 6.31, 8.86, 11.23, 13.79, 16.36, 18.93, 21.5, 23.88, 26.45};
	
	//interpolate all values between the above defined values
	uint32_t potiSize = (sizeof(potiOutputValues) / sizeof(potiOutputValues[0]));
	uint32_t voltageSize = (sizeof(voltageOutputValues) / sizeof(voltageOutputValues[0]));
	if (potiSize != voltageSize) {
		DEBUG_SERIAL.print("Invalid array size used for poti wiper computation");
	}

	float_t wiper = multiMap<float_t>(voltage, voltageOutputValues, potiOutputValues, potiSize);
	
	switch (psuType)
	{
		case PsuType::VCC_RAIL:
		{
			uint8_t wiperInt = static_cast<uint8_t>(wiper);
			wiperInt = min(wiperInt, 127);
			mcp_vcc.writeWiper(wiperInt);
			Debug.print(DBG_INFO, "\n>>> set wiper to = %d", wiperInt);
			break;
		}

		case PsuType::VDC_RAIL:
		{
			DEBUG_SERIAL.print("Not assembled yet");
			// uint8_t wiper = static_cast<uint8_t>(voltage * VDC_VOLTAGE_MULTIPLIER);
			// wiper = min(wiper, 127);
			// mcp_vdc.writeWiper(wiper);
			// Debug.print(DBG_INFO, "\n>>> set wiper to = %d", wiper);
			break;
		}

		default:
			DEBUG_SERIAL.print("Unsupported power rail");
			break;
	}
}


void HAL::setVccOutput(bool enable)
{
	mcp.digitalWrite(VCC_EN_PIN, enable);
}


void HAL::setVdcOutput(bool enable)
{
	mcp.digitalWrite(VDC_EN_PIN, enable);
}


void HAL::setDigitalOut(uint32_t outputChannel, enum DigOutMode mode)
{
	uint8_t dout_vcc = 0;
	uint8_t dout_vdc = 0;
	switch (outputChannel)
	{	
		case 1:
			dout_vcc = VCC_DOUT1_PIN;
			dout_vdc = VDC_DOUT1_PIN;
			break;
		
		case 2:
			dout_vcc = VCC_DOUT2_PIN;
			dout_vdc = VDC_DOUT2_PIN;
			break;

		case 3:
			dout_vcc = VCC_DOUT3_PIN;
			dout_vdc = VDC_DOUT3_PIN;
			break;

		case 4:
			dout_vcc = VCC_DOUT4_PIN;
			dout_vdc = VDC_DOUT4_PIN;
			break;

		default:
			DEBUG_SERIAL.print("Unsupported digital output channel");
			return;		
	}

	switch (mode)
	{	
		case DigOutMode::OFF:
			mcp.digitalWrite(dout_vcc, LOW);
			mcp.digitalWrite(dout_vdc, LOW);
			break;
		
		case DigOutMode::VCC_ON:
			mcp.digitalWrite(dout_vdc, LOW);
			delay(10); // delay to prevent shorting VCC and VDC
			mcp.digitalWrite(dout_vcc, HIGH);
			break;

		case DigOutMode::VDC_ON:
			mcp.digitalWrite(dout_vcc, LOW);
			delay(10); // delay to prevent shorting VCC and VDC
			mcp.digitalWrite(dout_vdc, HIGH);
			break;

		default:
			mcp.digitalWrite(dout_vcc, LOW);
			mcp.digitalWrite(dout_vdc, LOW);
			DEBUG_SERIAL.print("Unsupported digital output mode");
			break;		
	}
}


void HAL::initializeSerials(void) {
    // Serial init
	Serial.begin(SERIAL_BAUD);
	/*while(!Serial) {
		delay(1);
	}*/

	// UART0 init
	Serial1.setTX(UART0_TX);
	Serial1.setRX(UART0_RX);
	Serial1.begin(UART0_BAUD);

	// UART1 init
	Serial2.setTX(UART1_TX);
	Serial2.setRX(UART1_RX);
	Serial2.begin(UART1_BAUD);
}

void HAL::initializeDebugInterface(void) {
    // DEBUG init
	Debug.setDebugLevel(DEBUG_LEVEL);
	Debug.setDebugOutputStream(&DEBUG_SERIAL);
}

void HAL::initializeGpio(void) {
	// GPIO init
	pinMode(LED_PIN, OUTPUT);
	pinMode(RELAIS1_PIN, OUTPUT);
	pinMode(RELAIS2_PIN, OUTPUT);
	pinMode(LEVELSHIFT1_PIN, OUTPUT);
	pinMode(LEVELSHIFT2_PIN, OUTPUT);
	pinMode(LEVELSHIFT3_PIN, OUTPUT);
	pinMode(LEVELSHIFT4_PIN, OUTPUT);
	pinMode(AMP_HL, OUTPUT);
	pinMode(SD_CD, INPUT);

	digitalWrite(LED_PIN, HIGH);	// ToDo: do something usefull with led ...
	digitalWrite(RELAIS1_PIN, LOW);
	digitalWrite(RELAIS2_PIN, LOW);
	digitalWrite(LEVELSHIFT1_PIN, LOW);
	digitalWrite(LEVELSHIFT2_PIN, LOW);
	digitalWrite(LEVELSHIFT3_PIN, LOW);
	digitalWrite(LEVELSHIFT4_PIN, LOW);
	digitalWrite(AMP_HL, LOW);	// ToDo: use some default value
	digitalWrite(SD_CD, LOW);	// ToDo: use some default value
}

void HAL::initializeSpi(void) {
	// SPI init
	SPI.setSCK(SPI0_SCK);
	SPI.setTX(SPI0_MOSI);
	SPI.setRX(SPI0_MISO);

	ad.begin();
	ad.setMode(MD_AD9833::MODE_OFF);
	ad.setFrequency(MD_AD9833::CHAN_0, 0.0f);
	//MCP2515_CAN1_CS
	//MCP2515_CAN2_CS
	//SD_CS
}


void HAL::initializeI2c(void) {
	// I2C 1 init
	Wire.setSDA(I2C0_SDA);
	Wire.setSCL(I2C0_SCL);
	if (!ads.begin(ADS1115_AIN_ADDR)) {
		MENU_SERIAL.println("Failed to initialize ADS1115 (AIN)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize ADS1115 (AIN)");
	}

	if (!i2ceeprom.begin(EEPROM_ADDR)) {
		MENU_SERIAL.println("Failed to initialize EEPROM");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize EEPROM");
	}

	if (!mcp.begin_I2C(MCP23017_DOUT_ADDR)) {
		MENU_SERIAL.println("Failed to initialize MCP23017 (DOUT)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize MCP23017 (DOUT)");
	}

	if (!dac_bat.begin(MCP4725_BAT_ADDR)) {
		MENU_SERIAL.println("Failed to initialize MCP4725 (BAT)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize MCP4725 (BAT)");
	}
	dac_bat.setVoltage(0, false); // use bat.value*BAT_MULTIPLIER variable

	if (!dac_dds.begin(MCP4725_DDSOFFSET_ADDR)) {
		MENU_SERIAL.println("Failed to initialize MCP4725 (DDS offset)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize MCP4725 (DDS offset)");
	}
	dac_dds.setVoltage(0, false); // use fout.offset (needs to be converted)

	mcp_vcc.begin();
	mcp_vcc.writeWiper(255);	// ToDo: use correct init value

	// mcp_vdc.begin();
	// mcp_vdc.writeWiper(255);	// ToDo: use correct init value

	// I2C 2 init
	Wire1.setSDA(I2C1_SDA);
	Wire1.setSCL(I2C1_SCL);
}


void HAL::initializePsu(void) {
    // MCP23017 init
	for (int i = 0; i <= MCP23017_MAX_PIN; i++) {
		mcp.pinMode(i, OUTPUT);
		mcp.digitalWrite(i, LOW);
	}

	if (!ina_vcc.begin()) {
		MENU_SERIAL.println("Failed to initialize INA219 (VCC).");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize INA219 (VCC)");
	}
}