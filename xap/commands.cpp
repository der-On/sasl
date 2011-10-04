#include "commands.h"
#include "xpsdk.h"

// cstddef is needed on OS X to have NULL defined
#include <cstddef>

using namespace xap;


static SaslCommand findCommand(const char *name, void *cmdDta)
{
    return XPLMFindCommand(name);
}


static SaslCommand createCommand(const char *name, const char *description, 
        void *cmdDta)
{
    return XPLMCreateCommand(name, description);
}


static void addCommandHandler(SaslCommand command, sasl_command_callback handler, 
        int before, void *data, void *cmdDta)
{
    XPLMRegisterCommandHandler((XPLMCommandRef)command, handler, before, data);
}


static void removeCommandHandler(SaslCommand command, 
        sasl_command_callback handler, int before, void *data, void *cmdDta)
{
    XPLMUnregisterCommandHandler((XPLMCommandRef)command, handler, before,
            data);
}


static void commandBegin(SaslCommand command, void *cmdDta)
{
    XPLMCommandBegin((XPLMCommandRef)command);
}


static void commandEnd(SaslCommand command, void *cmdDta)
{
    XPLMCommandEnd((XPLMCommandRef)command);
}


static void commandOnce(SaslCommand command, void *cmdDta)
{
    XPLMCommandOnce((XPLMCommandRef)command);
}



void xap::registerCommandsApi(SASL sasl)
{
    SaslCommandCallbacks callbacks = { findCommand, createCommand,
        addCommandHandler, removeCommandHandler, commandBegin,
        commandEnd, commandOnce };
    sasl_set_commands(sasl, &callbacks, NULL);
}


