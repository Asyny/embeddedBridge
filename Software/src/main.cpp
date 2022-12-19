#include <Arduino.h>

// custom libraries
#include "hardware.h"
#include "HAL.h"

#include "CommandHandling/CommandHandler.h"
#include "CommandHandling/InteractiveCommandHandler.h"
#include "CommandHandling/ScpiCommandHandler.h"

#include "DataHandling/DataHandler.h"


CommandHandler* commandHandler = nullptr;
DataHandler* dataHandler = nullptr;

 void setup() {
	Debug.setDebugLevel(DEBUG_LEVEL);
	Debug.setDebugOutputStream(&DEBUG_SERIAL);

	HAL::getInstance()->initialize();

	commandHandler = new ScpiCommandHandler();
	dataHandler = new DataHandler();

	Debug.print(DBG_INFO, "\n>>> init done");
}

void selectCommandMenuSource(void) {
	if (DataModel::newMode != DataModel::CommandHandlerInputMode::IDLE) {
		if (commandHandler != nullptr)
				delete commandHandler;

		if (DataModel::newMode == DataModel::CommandHandlerInputMode::INTERACTIVE) {
			commandHandler = new SerialMenu::InteractiveCommandHandler();
		}
		else if (DataModel::newMode == DataModel::CommandHandlerInputMode::SCPI) {
			commandHandler = new ScpiCommandHandler();
		}

		commandHandler->setup();
		DataModel::newMode = DataModel::CommandHandlerInputMode::IDLE;
	}
}

void loop() {
	selectCommandMenuSource();

	dataHandler->process();

	commandHandler->process();
	delay(100);
}
