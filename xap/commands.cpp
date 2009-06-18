#include "commands.h"
#include "xpsdk.h"

// cstddef is needed on OS X to have NULL defined
#include <cstddef>

using namespace xap;


static XaCommand findCommand(const char *name, void *cmdDta)
{
    return XPLMFindCommand(name);
}


static XaCommand createCommand(const char *name, const char *description, 
        void *cmdDta)
{
    return XPLMCreateCommand(name, description);
}


static void addCommandHandler(XaCommand command, xa_command_callback handler, 
        int before, void *data, void *cmdDta)
{
    XPLMRegisterCommandHandler((XPLMCommandRef)command, handler, before, data);
}


static void removeCommandHandler(XaCommand command, 
        xa_command_callback handler, int before, void *data, void *cmdDta)
{
    XPLMUnregisterCommandHandler((XPLMCommandRef)command, handler, before,
            data);
}


static void commandBegin(XaCommand command, void *cmdDta)
{
    XPLMCommandBegin((XPLMCommandRef)command);
}


static void commandEnd(XaCommand command, void *cmdDta)
{
    XPLMCommandEnd((XPLMCommandRef)command);
}


static void commandOnce(XaCommand command, void *cmdDta)
{
    XPLMCommandOnce((XPLMCommandRef)command);
}



void xap::registerCommandsApi(XA xa)
{
    XaCommandCallbacks callbacks = { findCommand, createCommand,
        addCommandHandler, removeCommandHandler, commandBegin,
        commandEnd, commandOnce };
    xa_set_commands(xa, &callbacks, NULL);
}


