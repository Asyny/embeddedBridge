#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "DataHandling/DataModel.h"

class CommandHandler {

public:
    CommandHandler(void) = default; 
    virtual ~CommandHandler(void) = default;

    void setup(void);
    void process(void);

protected:
    virtual void doSetup(void) = 0;
    virtual void doProcess(void) = 0;
};

#endif // COMMAND_HANDLER_H