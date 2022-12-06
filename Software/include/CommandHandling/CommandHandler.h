#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

class CommandHandler {

public:
    CommandHandler(void) = default; 
    virtual ~CommandHandler(void) = default;

    virtual void setup(void) = 0;
    virtual void process(void) = 0;
};

#endif // COMMAND_HANDLER_H