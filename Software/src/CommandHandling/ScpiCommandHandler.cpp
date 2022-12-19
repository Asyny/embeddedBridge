#include <Arduino_DebugUtils.h>

#include "CommandHandling/ScpiCommandHandler.h"

#include "hardware.h"


ScpiCommandHandler::ScpiCommandHandler(void) : CommandHandler() {

}


ScpiCommandHandler::~ScpiCommandHandler(void) {
    
}


void ScpiCommandHandler::doSetup(void) {
    Debug.print(DBG_INFO, "\n>>> ScpiCommandHandler::setup");
}


void ScpiCommandHandler::doProcess(void) {
    String input = MENU_SERIAL.readString();
    input.replace("\r\n", "");
    if (input.equals("SYStem:COMmand:INput MENU")) {
        DataModel::newMode = DataModel::CommandHandlerInputMode::INTERACTIVE;
    }
}