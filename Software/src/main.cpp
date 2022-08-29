#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// third party libraries
#include "MultiMap.h"
#include <Arduino_DebugUtils.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_EEPROM_I2C.h>
#include <Adafruit_INA219.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP4728.h>
#include <Adafruit_MCP4725.h>
#include <MD_AD9833.h>
#include <MCP45HVX1.h>
#include <mcp2515.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

// custom libraries
#include "hardware.h"

#define DEBUG_SERIAL Serial1
#define DEBUG_LEVEL DBG_VERBOSE	// DBG_NONE, DBG_ERROR, DBG_WARNING, DBG_INFO, DBG_DEBUG, DBG_VERBOSE

using namespace Menu;

// v EEPROM MENU v ------------------------------------------------------------------------------------------
Adafruit_EEPROM_I2C i2ceeprom;

/*uint8_t test;
test = i2ceeprom.read(0x0);
MENU_SERIAL.print("eeprom 0x0 read1 is "); MENU_SERIAL.print(test);

test = 3;
i2ceeprom.write(0x0, test);
test = i2ceeprom.read(0x0);
MENU_SERIAL.print("eeprom 0x0 read2 is "); MENU_SERIAL.print(test);*/
// ^ EEPROM MENU ^ ------------------------------------------------------------------------------------------

// v AIN MENU v ------------------------------------------------------------------------------------------
Adafruit_ADS1115 ads;
struct ainSettingsSpecific {
	int16_t adc_value;
	char* adc_value_string = new char[8];	// xx.xxxV\n
};

struct ainSettings {
	uint8_t gain = 0;
	uint8_t mode = 0;
	struct ainSettingsSpecific ain1;
	struct ainSettingsSpecific ain2;
	struct ainSettingsSpecific ain3;
	struct ainSettingsSpecific ain4;
} ain;

void ainGainEvent() {
	switch(ain.gain) {
		case 0: MENU_SERIAL.println("auto gain not implemented yet!"); break; // ToDo: add auto gain
		case 1: ads.setGain(GAIN_TWOTHIRDS); break;
		case 2: ads.setGain(GAIN_ONE); break;
		case 3: ads.setGain(GAIN_TWO); break;
		case 4: ads.setGain(GAIN_FOUR); break;
		case 5: ads.setGain(GAIN_EIGHT); break;
		case 6: ads.setGain(GAIN_SIXTEEN); break;
		default: MENU_SERIAL.print("GAIN ERROR "); MENU_SERIAL.println(ain.gain); return;
	}
	Debug.print(DBG_INFO, "\n>>> gain set to = %d", ain.gain);
}

SELECT(ain.gain, ainGainMenu, "gain", ainGainEvent, exitEvent, noStyle
	,VALUE("auto", 0, doNothing, noEvent)
	,VALUE("1/3", 1, doNothing, noEvent)
	,VALUE("1", 2, doNothing, noEvent)
	,VALUE("2", 3, doNothing, noEvent)
	,VALUE("4", 4, doNothing, noEvent)
	,VALUE("8", 5, doNothing, noEvent)
	,VALUE("16", 6, doNothing, noEvent)
);

const char *constMEM empty[] MEMMODE = {};
MENU(ainMenu, "AIN", doNothing, noEvent, noStyle
	,SUBMENU(ainGainMenu)
	,EDIT("AIN1", ain.ain1.adc_value_string, empty, doNothing, noEvent, noStyle)
	,EDIT("AIN2", ain.ain2.adc_value_string, empty, doNothing, noEvent, noStyle)
	,EDIT("AIN3", ain.ain3.adc_value_string, empty, doNothing, noEvent, noStyle)
	,EDIT("AIN4", ain.ain4.adc_value_string, empty, doNothing, noEvent, noStyle)
	,EXIT("Back")
);
// ^ AIN MENU ^ ------------------------------------------------------------------------------------------

// v DOUT+PSU MENU v ------------------------------------------------------------------------------------------
Adafruit_INA219 ina_vcc(INA219_VCC_ADDR);
//Adafruit_INA219 ina_vdc(INA219_VDC_ADDR);
//Adafruit_INA219 ina_dout1(INA219_DOUT1_ADDR);
//Adafruit_INA219 ina_dout2(INA219_DOUT2_ADDR);
MCP45HVX1 mcp_vcc(MCP45HV31_VCC_ADDR);
Adafruit_MCP23X17 mcp;
struct inaValues {
	char* voltage = new char[7];	// xx.xxV\n
	char* current = new char[10];	// xxxx.xxmA\n
	char* power = new char[10];		// xxxx.xxmW\n
};

struct vccSettings {
	struct inaValues ina;
	float value = 0;
	bool enable = false;
};
struct vccSettings vcc;
struct vccSettings vdc;

void setPsu() {
	// ToDo: use constrain() https://www.arduino.cc/reference/en/language/functions/math/constrain/
	if(vcc.value < 0.0 || vcc.value > VCC_VDC_MAX_VOLTAGE) {
		MENU_SERIAL.print("ERROR setPsu: psu voltage "); MENU_SERIAL.print(vcc.value); MENU_SERIAL.println(" out of range!");
		return;
	}
	if(vdc.value < 0.0 || vdc.value > VCC_VDC_MAX_VOLTAGE) {
		MENU_SERIAL.print("ERROR setPsu: psu voltage "); MENU_SERIAL.print(vdc.value); MENU_SERIAL.println(" out of range!");
		return;
	}
	float poti_out[] = {12, 25, 38, 50, 63, 76, 89, 102, 114, 127};
	float vcc_output[]  = {4.87, 6.31, 8.86, 11.23, 13.79, 16.36, 18.93, 21.5, 23.88, 26.45};
	float vcc_wiper = multiMap<float>(vcc.value, vcc_output, poti_out, (sizeof(poti_out) / sizeof(poti_out[0])));
	//uint8_t vcc_wiper = vcc.value * VCC_VOLTAGE_MULTIPLIER;
	uint8_t vdc_wiper = vdc.value * VDC_VOLTAGE_MULTIPLIER;
	if(vcc_wiper > 127) { vcc_wiper = 127; }
	if(vdc_wiper > 127) { vdc_wiper = 127; }
	mcp_vcc.writeWiper(vcc_wiper);
	//mcp_vdc.writeWiper(vdc_wiper);

	Debug.print(DBG_INFO, "\n>>> set vcc_wiper to = %.2f", vcc_wiper);
	Debug.print(DBG_INFO, "\n>>> set vdc_wiper to = %d", vdc_wiper);
}

void psuEvent() {
	mcp.digitalWrite(VCC_EN_PIN, vcc.enable);
	mcp.digitalWrite(VDC_EN_PIN, vdc.enable);
}

TOGGLE(vcc.enable, vccEnMenu, "output ", psuEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
TOGGLE(vdc.enable, vdcEnMenu, "output ", psuEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
MENU(vccMenu, "VCC", doNothing, noEvent, noStyle
	,SUBMENU(vccEnMenu)
	,FIELD(vcc.value, "set", "V", 0.0,VCC_VDC_MAX_VOLTAGE,1.0,0.1, setPsu, exitEvent, noStyle)
	,EDIT("voltage", vcc.ina.voltage, empty, doNothing, noEvent, noStyle)
	,EDIT("current", vcc.ina.current, empty, doNothing, noEvent, noStyle)
	,EDIT("power", vcc.ina.power, empty, doNothing, noEvent, noStyle)
);
/*MENU(vdcMenu, "VDC", doNothing, noEvent, noStyle
	,SUBMENU(vdcEnMenu)
	,FIELD(vdc.value, "set", "V", 0.0,VCC_VDC_MAX_VOLTAGE,1.0,0.1, setPsu, exitEvent, noStyle)
	,EDIT("voltage", vdc.ina.voltage, empty, doNothing, noEvent, noStyle)
	,EDIT("current", vdc.ina.current, empty, doNothing, noEvent, noStyle)
	,EDIT("power", vdc.ina.power, empty, doNothing, noEvent, noStyle)
);*/

// ToDo: add INA219
struct doutSettings {
	uint8_t value = 0;
};

struct doutSettings dout1;
struct doutSettings dout2;
struct doutSettings dout3;
struct doutSettings dout4;

void setDout(int dout_vcc, int dout_vdc, int value) {
	if(value == 1) {
		mcp.digitalWrite(dout_vdc, LOW);
		delay(10); // delay to prevent shorting VCC and VDC
		mcp.digitalWrite(dout_vcc, HIGH);
	} else if (value == 2) {
		mcp.digitalWrite(dout_vcc, LOW);
		delay(10); // delay to prevent shorting VCC and VDC
		mcp.digitalWrite(dout_vdc, HIGH);
	} else {
		mcp.digitalWrite(dout_vcc, LOW);
		mcp.digitalWrite(dout_vdc, LOW);
	}
}

void dout1Event() { setDout(VCC_DOUT1_PIN, VDC_DOUT1_PIN, dout1.value); }
void dout2Event() { setDout(VCC_DOUT2_PIN, VDC_DOUT2_PIN, dout2.value); }
void dout3Event() { setDout(VCC_DOUT3_PIN, VDC_DOUT3_PIN, dout3.value); }
void dout4Event() { setDout(VCC_DOUT4_PIN, VDC_DOUT4_PIN, dout4.value); }
SELECT(dout1.value, dout1Menu, "Dout1", dout1Event, exitEvent, noStyle
	,VALUE("VDC", 2, doNothing, noEvent)
	,VALUE("VCC", 1, doNothing, noEvent)
	,VALUE("Off", 0, doNothing, noEvent)
);
SELECT(dout2.value, dout2Menu, "Dout2", dout2Event, exitEvent, noStyle
	,VALUE("VDC", 2, doNothing, noEvent)
	,VALUE("VCC", 1, doNothing, noEvent)
	,VALUE("Off", 0, doNothing, noEvent)
);
SELECT(dout3.value, dout3Menu, "Dout3", dout3Event, exitEvent, noStyle
	,VALUE("VDC", 2, doNothing, noEvent)
	,VALUE("VCC", 1, doNothing, noEvent)
	,VALUE("Off", 0, doNothing, noEvent)
);
SELECT(dout4.value, dout4Menu, "Dout4", dout4Event, exitEvent, noStyle
	,VALUE("VDC", 2, doNothing, noEvent)
	,VALUE("VCC", 1, doNothing, noEvent)
	,VALUE("Off", 0, doNothing, noEvent)
);
MENU(doutMenu, "DOUT", doNothing, noEvent, noStyle
	,SUBMENU(vccMenu)
	,EDIT("vcc voltage", vcc.ina.voltage, empty, doNothing, noEvent, noStyle)
//	,SUBMENU(vdcMenu)
//	,EDIT("vdc voltage", vdc.ina.voltage, empty, doNothing, noEvent, noStyle)
	,SUBMENU(dout1Menu)
	,SUBMENU(dout2Menu)
	,SUBMENU(dout3Menu)
	,SUBMENU(dout4Menu)
	,EXIT("Back")
);
// ^ DOUT+PSU MENU ^ ------------------------------------------------------------------------------------------

// v FOUT MENU v ------------------------------------------------------------------------------------------
MD_AD9833 AD(SPI0_MOSI, SPI0_SCK, AD9833_DDS_CS);
Adafruit_MCP4725 dac_dds;

struct foutSettings {
	char frequency[50]="1000";
	float offset = 0.0;
	uint8_t mode = 0;
	uint8_t multiplier = 0;
} fout;

static MD_AD9833::mode_t modes[] = {
	MD_AD9833::MODE_OFF,
	MD_AD9833::MODE_SINE,
	MD_AD9833::MODE_SQUARE1,
	MD_AD9833::MODE_SQUARE2,
	MD_AD9833::MODE_TRIANGLE 
};

const char* constMEM alphaNum MEMMODE="0123456789";
const char* constMEM alphaNumMask[] MEMMODE={alphaNum};

void foutEvent() {
	uint multip = 1000;
	for(int i = 1; i < fout.multiplier; i++) {
		multip *= multip;
	}

	uint freq = atoi(fout.frequency);
	if(fout.multiplier != 0) {
		freq *= multip;
	}

	AD.setMode(modes[fout.mode]);
	AD.setFrequency(MD_AD9833::CHAN_0, freq);
}

void foutOffsetEvent() {
	int offset_int = fout.offset * FOUT_OFFSET_MULTIPLIER;
	dac_dds.setVoltage(offset_int, false);
}

SELECT(fout.mode, foutModeMenu, "Mode", foutEvent, exitEvent, noStyle
	,VALUE("Off", 0, doNothing, noEvent)
	,VALUE("Sine", 1, doNothing, noEvent)
	,VALUE("Square1", 2, doNothing, noEvent)
	,VALUE("Square2", 3, doNothing, noEvent)
	,VALUE("Triangle", 4, doNothing, noEvent)
);

SELECT(fout.multiplier, foutFreqMultipMenu, "Multiplier", foutEvent, exitEvent, noStyle
	,VALUE("Hz",  0, doNothing, noEvent)
	,VALUE("kHz", 1, doNothing, noEvent)
	,VALUE("MHz", 2, doNothing, noEvent)
);

MENU(foutMenu, "FOUT", doNothing, noEvent, noStyle
	,SUBMENU(foutModeMenu)
	,EDIT("Frequency", fout.frequency, alphaNumMask, foutEvent, exitEvent, noStyle)
	,SUBMENU(foutFreqMultipMenu)
	,FIELD(fout.offset, "offset", "V", 0.0,12.0,1.0,0.1, foutOffsetEvent, exitEvent, noStyle)
	,EXIT("Back")
);
// ^ FOUT MENU ^

// v BAT MENU v
Adafruit_MCP4725 dac_bat;
//Adafruit_INA219 ina_bat(INA219_BAT_ADDR);
struct batSettings {
	float value = 0.0;
	struct inaValues ina;
} bat;

void batEvent() {
	int bat_out = bat.value * BAT_MULTIPLIER;
	if(bat_out > 4095) { bat_out = 4095; }
	dac_bat.setVoltage(bat_out, false);
}

MENU(batMenu, "Battery sim", doNothing, noEvent, noStyle
	,FIELD(bat.value, "set", "V", 0.0,3.6,1.0,0.1, batEvent, exitEvent, noStyle)
	,EXIT("Back")
);
// ^ BAT MENU ^

// v AOUT MENU v
// ^ AOUT MENU ^

// v CAN MENU v
// can terminal only in main manu?
// ^ CAN MENU ^

// v RS232 MENU v
// rs232 terminal only in main manu?
// ^ RS232 MENU ^

// v LSHIFT MENU v
struct lshiftSettings {
	bool enable1 = false;
	bool enable2 = false;
	bool enable3 = false;
	bool enable4 = false;
} lshift;

void lshiftEvent() {
	digitalWrite(LEVELSHIFT1_PIN, lshift.enable1);
	digitalWrite(LEVELSHIFT2_PIN, lshift.enable2);
	digitalWrite(LEVELSHIFT3_PIN, lshift.enable3);
	digitalWrite(LEVELSHIFT4_PIN, lshift.enable4);
}
TOGGLE(lshift.enable1, lshiftEn1Menu, "1 ", lshiftEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
TOGGLE(lshift.enable2, lshiftEn2Menu, "2 ", lshiftEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
TOGGLE(lshift.enable3, lshiftEn3Menu, "3 ", lshiftEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
TOGGLE(lshift.enable4, lshiftEn4Menu, "4 ", lshiftEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
MENU(lshiftMenu, "Level Shift", doNothing, noEvent, noStyle
	,SUBMENU(lshiftEn1Menu)
	,SUBMENU(lshiftEn2Menu)
	,SUBMENU(lshiftEn3Menu)
	,SUBMENU(lshiftEn4Menu)
	,EXIT("Back")
);
// ^ LSHIFT MENU ^

// v ESP8266 MENU v
// ToDo: what to do????
// ^ ESP8266 MENU ^

// v SD MENU v
// ToDo: what to do????
// ^ SD MENU ^

// v EEPROM MENU v
// ToDo: what to do????
// ^ EEPROM MENU ^

// v RELAIS MENU v
struct relaisSettings {
	bool enable1 = false;
	bool enable2 = false;
} relais;

void relaisEvent() {
	digitalWrite(RELAIS1_PIN, relais.enable1);
	digitalWrite(RELAIS2_PIN, relais.enable2);
}

TOGGLE(relais.enable1, relaisEn1Menu, "1 ", relaisEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
TOGGLE(relais.enable2, relaisEn2Menu, "2 ", relaisEvent, enterEvent, wrapStyle
	,VALUE("on", HIGH, doNothing, noEvent)
	,VALUE("off", LOW, doNothing, noEvent)
);
MENU(relaisMenu, "Relais", doNothing, noEvent, noStyle
	,SUBMENU(relaisEn1Menu)
	,SUBMENU(relaisEn2Menu)
	,EXIT("Back")
);
// ^ RELAIS MENU ^

// v MAIN MENU v
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
	,SUBMENU(doutMenu)
	,SUBMENU(lshiftMenu)
	,SUBMENU(ainMenu)
	,SUBMENU(foutMenu)
	,SUBMENU(batMenu)
	,SUBMENU(relaisMenu)
	// ToDo: calibration for AIN, ?
	,EXIT("EXIT")
);

#define MAX_DEPTH 10
const navCodesDef myCodes={
	{noCmd,(char)0xff},
	{escCmd, 'a'},
	{enterCmd, 'd'},
	{downCmd, 'w'},
	{upCmd, 's'},
	{leftCmd, 'a'},    // doesn't do anything anyway...
	{rightCmd, 'd'},   // doesn't do anything anyway...
	{idxCmd, '?'},     // doesn't do anything anyway...
	{scrlUpCmd, 0x35}, // doesn't do anything anyway...
	{scrlUpCmd, 0x36}  // doesn't do anything anyway...
};
const navCodesDef newNavCodes={
	{noCmd,(uint8_t)0xff},
	{escCmd,'/'},
	{enterCmd,'*'},
	{upCmd,'+'},
	{downCmd,'-'},
	{leftCmd,'-'},
	{rightCmd,'+'},
	{idxCmd,'?'},
	{scrlUpCmd,0x35},
	{scrlUpCmd,0x36}
};
config myOptions('>','-',myCodes,true);
serialIn serial(MENU_SERIAL);
MENU_OUTPUTS(out,MAX_DEPTH,SERIAL_OUT(MENU_SERIAL),NONE);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);
// ^ MAIN MENU ^

void setup() {
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

	// DEBUG init
	Debug.setDebugLevel(DEBUG_LEVEL);
	Debug.setDebugOutputStream(&DEBUG_SERIAL);

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
	digitalWrite(RELAIS1_PIN, relais.enable1);
	digitalWrite(RELAIS2_PIN, relais.enable2);
	digitalWrite(LEVELSHIFT1_PIN, lshift.enable1);
	digitalWrite(LEVELSHIFT2_PIN, lshift.enable2);
	digitalWrite(LEVELSHIFT3_PIN, lshift.enable3);
	digitalWrite(LEVELSHIFT4_PIN, lshift.enable4);
	digitalWrite(AMP_HL, LOW);	// ToDo: use some default value
	digitalWrite(SD_CD, LOW);	// ToDo: use some default value

	// SPI init
	SPI.setSCK(SPI0_SCK);
	SPI.setTX(SPI0_MOSI);
	SPI.setRX(SPI0_MISO);

	AD.begin();
	AD.setMode(modes[fout.mode]);
	AD.setFrequency(MD_AD9833::CHAN_0, atoi(fout.frequency));
	//MCP2515_CAN1_CS
	//MCP2515_CAN2_CS
	//SD_CS

	// I2C 1 init
	Wire.setSDA(I2C0_SDA);
	Wire.setSCL(I2C0_SCL);
	if(!ads.begin(ADS1115_AIN_ADDR)) {
		MENU_SERIAL.println("Failed to initialize ADS1115 (AIN)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize ADS1115 (AIN)");
	}

	if(!i2ceeprom.begin(EEPROM_ADDR)) {
		MENU_SERIAL.println("Failed to initialize EEPROM");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize EEPROM");
	}

	if(!mcp.begin_I2C(MCP23017_DOUT_ADDR)) {
		MENU_SERIAL.println("Failed to initialize MCP23017 (DOUT)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize MCP23017 (DOUT)");
	}

	if(!dac_bat.begin(MCP4725_BAT_ADDR)) {
		MENU_SERIAL.println("Failed to initialize MCP4725 (BAT)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize MCP4725 (BAT)");
	}
	dac_bat.setVoltage(0, false); // use bat.value*BAT_MULTIPLIER variable

	if(!dac_dds.begin(MCP4725_DDSOFFSET_ADDR)) {
		MENU_SERIAL.println("Failed to initialize MCP4725 (DDS offset)");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize MCP4725 (DDS offset)");
	}
	dac_dds.setVoltage(0, false); // use fout.offset (needs to be converted)

	mcp_vcc.begin();
	mcp_vcc.writeWiper(255);	// ToDo: use correct init value

	// I2C 2 init
	Wire1.setSDA(I2C1_SDA);
	Wire1.setSCL(I2C1_SCL);

	// MCP23017 init
	for(int i = 0; i <= MCP23017_MAX_PIN; i++) {
		mcp.pinMode(i, OUTPUT);
		mcp.digitalWrite(i, LOW);
	}

	if(!ina_vcc.begin()) {
		MENU_SERIAL.println("Failed to initialize INA219 (VCC).");
		Debug.print(DBG_ERROR, "\n>>> Failed to initialize INA219 (VCC)");
	}

	options=&myOptions;

	ainMenu[1].disable();
	ainMenu[2].disable();
	ainMenu[3].disable();
	ainMenu[4].disable();

	doutMenu[1].disable();	// shows current output voltage of vcc
	//doutMenu[3].disable();	// shows current output voltage of vdc

	vccMenu[2].disable();	// shows current output voltage of vcc
	vccMenu[3].disable();	// shows current output current of vcc
	vccMenu[4].disable();	// shows current output power of vcc

/*	vdcMenu[2].disable();	// shows current output voltage of vdc
	vdcMenu[3].disable();	// shows current output current of vdc
	vdcMenu[4].disable();	// shows current output power of vdc
*/

	Debug.print(DBG_INFO, "\n>>> init done");
}

void loop() {
	sprintf(vcc.ina.voltage, "%.2fV", ina_vcc.getBusVoltage_V());
	sprintf(vcc.ina.current, "%.2fmA", ina_vcc.getCurrent_mA());
	sprintf(vcc.ina.power, "%.2fmW", ina_vcc.getPower_mW());

	/*sprintf(vdc.ina.voltage, "%.2fV", ina_vdc.getBusVoltage_V());
	sprintf(vdc.ina.current, "%.2fmA", ina_vdc.getCurrent_mA());
	sprintf(vdc.ina.power, "%.2fmW", ina_vdc.getPower_mW());*/

	ain.ain1.adc_value = ads.readADC_SingleEnded(0);
	ain.ain2.adc_value = ads.readADC_SingleEnded(1);
	ain.ain3.adc_value = ads.readADC_SingleEnded(2);
	ain.ain4.adc_value = ads.readADC_SingleEnded(3);

	sprintf(ain.ain1.adc_value_string, "%.3fV", ads.computeVolts(ain.ain1.adc_value) * AIN4_VOLTAGE_DEVIDER);
	sprintf(ain.ain2.adc_value_string, "%.3fV", ads.computeVolts(ain.ain2.adc_value) * AIN4_VOLTAGE_DEVIDER);
	sprintf(ain.ain3.adc_value_string, "%.3fV", ads.computeVolts(ain.ain3.adc_value) * AIN4_VOLTAGE_DEVIDER);
	sprintf(ain.ain4.adc_value_string, "%.3fV", ads.computeVolts(ain.ain4.adc_value) * AIN4_VOLTAGE_DEVIDER);

	//Serial1.println("asdf");
	//Serial2.println("bsdf");

	nav.poll();
	delay(100);
}
