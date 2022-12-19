#include <Arduino_DebugUtils.h>

#include "CommandHandling/ScpiCommandHandler.h"

#include "hardware.h"

ScpiCommandHandler::ScpiCommandHandler(void) : CommandHandler() {

}


ScpiCommandHandler::~ScpiCommandHandler(void) {
    
}


void ScpiCommandHandler::doSetup(void) {
    Debug.print(DBG_INFO, "\n>>> ScpiCommandHandler::setup");
    // first read from serial interface always contains "\374" at the beginning
    // clearing this content by reading serial interface
    String input = MENU_SERIAL.readString();
}


void ScpiCommandHandler::doProcess(void) {
    String input = MENU_SERIAL.readString();
    if (!input.endsWith("\r\n")) {
        commandLine += input;
    }
    else {
        commandLine += input;
        commandLine.replace("\r\n", "");
        if (commandLine.equals("SYStem:COMmand:INput MENU")) {
            DataModel::newMode = DataModel::CommandHandlerInputMode::INTERACTIVE;
        }
        if (commandLine.equals("RELais:A:ACTivate")) {
            DataModel::relais.enable1 = true;
        }
        if (commandLine.equals("RELais:A:DEACTivate")) {
            DataModel::relais.enable1 = false;
        }
        if (commandLine.equals("RELais:B:ACTivate")) {
            DataModel::relais.enable2 = true;
        }
        if (commandLine.equals("RELais:B:DEACTivate")) {
            DataModel::relais.enable2 = false;
        }
        commandLine = "";
    }
}