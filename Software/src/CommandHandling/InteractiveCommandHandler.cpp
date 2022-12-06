#include "CommandHandling/InteractiveCommandHandler.h"

#include "hardware.h"
#include "HAL.h"


SerialMenu::InteractiveCommandHandler::InteractiveCommandHandler(void) : CommandHandler()
{
}


SerialMenu::InteractiveCommandHandler::~InteractiveCommandHandler(void) {
    
}


void SerialMenu::InteractiveCommandHandler::setup(void) {
    options = &myOptions;
    initialize();
}


void SerialMenu::InteractiveCommandHandler::process(void) {
    navInstance->poll();
}