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

    void commandProcessing(void);
    void relaisCommandProcessing(void);
    void levelShiftCommandProcessing(void);
    void adcCommandProcessing(void);
    void batSimCommandProcessing(void);
    void fOutCommandProcessing(void);
    void dOutCommandProcessing(void);
};

#endif // SCPI_COMMAND_HANDLER_H