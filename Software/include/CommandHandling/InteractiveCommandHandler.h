#ifndef INTERACTIVE_COMMAND_HANDLER_H
#define INTERACTIVE_COMMAND_HANDLER_H

#include "CommandHandler.h"

#include <Arduino_DebugUtils.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

#include "hardware.h"
#include "HAL.h"
#include "DataHandling/DataModel.h"

namespace SerialMenu {
    class InteractiveCommandHandler : public CommandHandler {

    public:
        InteractiveCommandHandler(void); 
        virtual ~InteractiveCommandHandler(void);

        virtual void setup(void) override;
        virtual void process(void) override;
    };
}

#endif // INTERACTIVE_COMMAND_HANDLER_H