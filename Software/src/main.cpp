#include <Arduino.h>

// third party libraries
#include <Arduino_DebugUtils.h>

// custom libraries
#include "hardware.h"
#include "HAL.h"
#include "CommandHandling/InteractiveCommandHandler.h"


#define DEBUG_SERIAL Serial1
#define DEBUG_LEVEL DBG_VERBOSE	// DBG_NONE, DBG_ERROR, DBG_WARNING, DBG_INFO, DBG_DEBUG, DBG_VERBOSE

CommandHandler* commandHandler = nullptr;

void setup() {
	HAL::getInstance()->initialize();

	commandHandler = new SerialMenu::InteractiveCommandHandler();
	commandHandler->setup();
	// ainMenu[1].disable();
	// ainMenu[2].disable();
	// ainMenu[3].disable();
	// ainMenu[4].disable();

	// doutMenu[1].disable();	// shows current output voltage of vcc
	// //doutMenu[3].disable();	// shows current output voltage of vdc

	// vccMenu[2].disable();	// shows current output voltage of vcc
	// vccMenu[3].disable();	// shows current output current of vcc
	// vccMenu[4].disable();	// shows current output power of vcc

/*	vdcMenu[2].disable();	// shows current output voltage of vdc
	vdcMenu[3].disable();	// shows current output current of vdc
	vdcMenu[4].disable();	// shows current output power of vdc
*/

	Debug.print(DBG_INFO, "\n>>> init done");
}

void loop() {
	// sprintf(vcc.ina.voltage, "%.2fV", ina_vcc.getBusVoltage_V());
	// sprintf(vcc.ina.current, "%.2fmA", ina_vcc.getCurrent_mA());
	// sprintf(vcc.ina.power, "%.2fmW", ina_vcc.getPower_mW());

	/*sprintf(vdc.ina.voltage, "%.2fV", ina_vdc.getBusVoltage_V());
	sprintf(vdc.ina.current, "%.2fmA", ina_vdc.getCurrent_mA());
	sprintf(vdc.ina.power, "%.2fmW", ina_vdc.getPower_mW());*/

	float ainVoltage = 0.0f;
	HAL::getInstance()->readAdcChannel(0, ainVoltage);
	// sprintf(SerialMenu::ain.ain1.adc_value_string, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(1, ainVoltage);
	// sprintf(SerialMenu::ain.ain2.adc_value_string, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(2, ainVoltage);
	// sprintf(SerialMenu::ain.ain3.adc_value_string, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(3, ainVoltage);
	// sprintf(SerialMenu::ain.ain4.adc_value_string, "%.3fV", ainVoltage);

	//Serial1.println("asdf");
	//Serial2.println("bsdf");

	//nav.poll();
	// TODO delete object!
	commandHandler->process();
	delay(100);
}
