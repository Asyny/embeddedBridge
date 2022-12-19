#ifndef SCPI_COMMAND_HANDLER_H
#define SCPI_COMMAND_HANDLER_H


#include "CommandHandler.h"
#include "DataHandling/DataModel.h"

class ScpiCommandHandler : public CommandHandler {

public:
    ScpiCommandHandler(void); 
    virtual ~ScpiCommandHandler(void);

    virtual void doSetup(void) override;
    virtual void doProcess(void) override;

private:
    String commandLine = "";

};

#endif // SCPI_COMMAND_HANDLER_H