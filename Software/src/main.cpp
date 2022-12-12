#include <Arduino.h>

// third party libraries
#include <Arduino_DebugUtils.h>

// custom libraries
#include "hardware.h"
#include "HAL.h"
#include "CommandHandling/InteractiveCommandHandler.h"

#include "DataHandling/DataModel.h"

#define DEBUG_SERIAL Serial1
#define DEBUG_LEVEL DBG_VERBOSE	// DBG_NONE, DBG_ERROR, DBG_WARNING, DBG_INFO, DBG_DEBUG, DBG_VERBOSE

CommandHandler* commandHandler = nullptr;

void setup() {
	HAL::getInstance()->initialize();

	commandHandler = new SerialMenu::InteractiveCommandHandler();
	commandHandler->setup();

	Debug.print(DBG_INFO, "\n>>> init done");
}

void loop() {
	// sprintf(vcc.ina.voltage, "%.2fV", ina_vcc.getBusVoltage_V());
	// sprintf(vcc.ina.current, "%.2fmA", ina_vcc.getCurrent_mA());
	// sprintf(vcc.ina.power, "%.2fmW", ina_vcc.getPower_mW());
	sprintf(DataModel::vcc.ina.voltage, "1V");
	sprintf(DataModel::vcc.ina.current, "1mA");
	sprintf(DataModel::vcc.ina.power, "1mW");

	// sprintf(vdc.ina.voltage, "%.2fV", ina_vdc.getBusVoltage_V());
	// sprintf(vdc.ina.current, "%.2fmA", ina_vdc.getCurrent_mA());
	// sprintf(vdc.ina.power, "%.2fmW", ina_vdc.getPower_mW());

	float ainVoltage = 0.0f;
	HAL::getInstance()->readAdcChannel(0, ainVoltage); 
	sprintf(DataModel::ain.adc1_value, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(1, ainVoltage);
	sprintf(DataModel::ain.adc2_value, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(2, ainVoltage);
	sprintf(DataModel::ain.adc3_value, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(3, ainVoltage);
	sprintf(DataModel::ain.adc4_value, "%.3fV", ainVoltage);

	commandHandler->process();
	delay(100);
}
