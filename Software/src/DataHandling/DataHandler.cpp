#include "DataHandling/DataHandler.h"
#include "DataHandling/DataModel.h"

#include "HAL.h"

DataHandler::DataHandler(void) {

}


DataHandler::~DataHandler(void) {
    
}


void DataHandler::process(void) {
	float_t inaValue = 0.0f;
	inaValue = HAL::getInstance()->getInaVoltage(HAL::PsuType::VCC_RAIL);
    sprintf(DataModel::vcc.ina.voltage, "%.2fV", inaValue);
	inaValue = HAL::getInstance()->getInaCurrent(HAL::PsuType::VCC_RAIL);
	sprintf(DataModel::vcc.ina.current, "%.2fmA", inaValue);
	inaValue = HAL::getInstance()->getInaPower(HAL::PsuType::VCC_RAIL);
	sprintf(DataModel::vcc.ina.power, "%.2fmW", inaValue);

	// inaValue = HAL::getInstance()->getInaVoltage(HAL::PsuType::VDC_RAIL);
    // sprintf(DataModel::vcc.ina.voltage, "%.2fV", inaValue);
	// inaValue = HAL::getInstance()->getInaCurrent(HAL::PsuType::VDC_RAIL);
	// sprintf(DataModel::vcc.ina.current, "%.2fmA", inaValue);
	// inaValue = HAL::getInstance()->getInaPower(HAL::PsuType::VDC_RAIL);
	// sprintf(DataModel::vcc.ina.power, "%.2fmW", inaValue);

    float_t ainVoltage = 0.0f;
	HAL::getInstance()->readAdcChannel(0, ainVoltage); 
	sprintf(DataModel::ain.adc1_value, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(1, ainVoltage);
	sprintf(DataModel::ain.adc2_value, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(2, ainVoltage);
	sprintf(DataModel::ain.adc3_value, "%.3fV", ainVoltage);
	HAL::getInstance()->readAdcChannel(3, ainVoltage);
	sprintf(DataModel::ain.adc4_value, "%.3fV", ainVoltage);
}