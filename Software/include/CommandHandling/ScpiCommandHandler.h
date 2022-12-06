#ifndef SCPI_COMMAND_HANDLER_H
#define SCPI_COMMAND_HANDLER_H


#include "CommandHandler.h"

class ScpiCommandHandler : public CommandHandler {

public:
    ScpiCommandHandler(void); 
    virtual ~ScpiCommandHandler(void);

    virtual void setup(void) override;
    virtual void process(void) override;
};

#endif // SCPI_COMMAND_HANDLER_H