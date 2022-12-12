#include <Arduino.h>

// third party libraries
#include <Arduino_DebugUtils.h>

// custom libraries
#include "hardware.h"
#include "HAL.h"
#include "CommandHandling/InteractiveCommandHandler.h"
#include "DataHandling/DataHandler.h"

#define DEBUG_SERIAL Serial1
#define DEBUG_LEVEL DBG_VERBOSE	// DBG_NONE, DBG_ERROR, DBG_WARNING, DBG_INFO, DBG_DEBUG, DBG_VERBOSE

CommandHandler* commandHandler = nullptr;
DataHandler* dataHandler = nullptr;

void setup() {
	HAL::getInstance()->initialize();

	commandHandler = new SerialMenu::InteractiveCommandHandler();
	commandHandler->setup();

	dataHandler = new DataHandler();

	Debug.print(DBG_INFO, "\n>>> init done");
}

void loop() {
	dataHandler->process();

	commandHandler->process();
	delay(100);
}
