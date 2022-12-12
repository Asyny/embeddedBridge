#include "DataHandling/DataHandler.h"
#include "DataHandling/DataModel.h"

#include "HAL.h"

DataHandler::DataHandler(void) {

}


DataHandler::~DataHandler(void) {
    
}


void DataHandler::process(void) {
    // sprintf(vcc.ina.voltage, "%.2fV", ina_vcc.getBusVoltage_V());
	// sprintf(vcc.ina.current, "%.2fmA", ina_vcc.getCurrent_mA());
	// sprintf(vcc.ina.power, "%.2fmW", ina_vcc.getPower_mW());

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
}