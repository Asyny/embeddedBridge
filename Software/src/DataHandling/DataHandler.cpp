#include "DataHandling/DataHandler.h"
#include "DataHandling/DataModel.h"

#include "HAL.h"
#include "hardware.h"

DataHandler::DataHandler(void) {

}


DataHandler::~DataHandler(void) {
    
}


void DataHandler::process(void) {

	updateAnalogInputGain();
	updateAdcValues();

	updateRelais();
	updateLevelShift();
	updateFrequencyOutput();
	updateBatterySimulation();

	updatePsu();
	updatePsuMonitoringValues();
	updateDigitalOutputs();
}


void DataHandler::updateRelais(void) {
	HAL::getInstance()->setRelais1(DataModel::relais.enable1);
    HAL::getInstance()->setRelais2(DataModel::relais.enable2);
}


void DataHandler::updateLevelShift() {
    HAL::getInstance()->setLevelShift1(DataModel::lshift.enable1);
    HAL::getInstance()->setLevelShift2(DataModel::lshift.enable2);
    HAL::getInstance()->setLevelShift3(DataModel::lshift.enable3);
    HAL::getInstance()->setLevelShift4(DataModel::lshift.enable4);
}


void DataHandler::updateAdcValues(void) {
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


void DataHandler::updatePsuMonitoringValues(void) {
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
}

void DataHandler::updatePsu() {
    HAL::getInstance()->configurePowerSupply(HAL::PsuType::VCC_RAIL, DataModel::vcc.value);
    HAL::getInstance()->configurePowerSupply(HAL::PsuType::VDC_RAIL, DataModel::vdc.value);
    HAL::getInstance()->setVccOutput(DataModel::vcc.enable);
    HAL::getInstance()->setVdcOutput(DataModel::vdc.enable);
}


void DataHandler::updateDigitalOutputs() { 
	HAL::getInstance()->setDigitalOut(1, static_cast<HAL::DigOutMode>(DataModel::dout1.value)); 
	HAL::getInstance()->setDigitalOut(2, static_cast<HAL::DigOutMode>(DataModel::dout2.value)); 
	HAL::getInstance()->setDigitalOut(3, static_cast<HAL::DigOutMode>(DataModel::dout3.value)); 
	HAL::getInstance()->setDigitalOut(4, static_cast<HAL::DigOutMode>(DataModel::dout4.value)); 
}

void DataHandler::updateFrequencyOutput() {
    HAL* hal = HAL::getInstance();
    hal->setFreqOutMode(hal->getFreqOutModeMapping(DataModel::fout.mode));
    hal->setFreqOutFrequency(atoi(DataModel::fout.frequency), DataModel::fout.multiplier);
    hal->setFreqOutOffset(DataModel::fout.offset);
}


void DataHandler::updateBatterySimulation() {
    // HAL::getInstance()->setBatSimVoltage(bat.value);
}


void DataHandler::updateAnalogInputGain() {
    switch(DataModel::ain.gain) {
        case 0: 
			Debug.print(DBG_INFO, "auto gain not implemented yet!\n"); 
			break; // ToDo: add auto gain

        case 1: 
			HAL::getInstance()->setAinGain(GAIN_TWOTHIRDS); 
			break;

        case 2: 
			HAL::getInstance()->setAinGain(GAIN_ONE); 
			break;
		
        case 3: 
			HAL::getInstance()->setAinGain(GAIN_TWO); 
			break;

        case 4: 
			HAL::getInstance()->setAinGain(GAIN_FOUR); 
			break;

        case 5: 
			HAL::getInstance()->setAinGain(GAIN_EIGHT); 
			break;

        case 6: 
			HAL::getInstance()->setAinGain(GAIN_SIXTEEN); 
			break;

        default:
			Debug.print(DBG_INFO, "GAIN ERROR %d\n", DataModel::ain.gain); 
			return;
    }
}