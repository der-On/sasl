#ifndef __COMMANDS_H__
#define __COMMANDS_H__


#include "luna.h"
#include "libavcallbacks.h"
#include <list>


namespace xa {

/// Commands container
class Commands
{
    private:
        /// Commands callbacks from simulator
        SaslCommandCallbacks callbacks;

        /// Reference to Lua
        Luna &lua;

        /// Data for commands api
        void *data;
        
    public:
        /// Lua command handler description
        struct CommandHandler
        {
            /// command to handle
            SaslCommand command;

            /// reference to commands object
            Commands *commands;

            /// When to call: before or after default handler
            int before;

            /// reference to callback
            int callbackRef;
        };

    private:
        /// List of registered commands
        std::list<CommandHandler> commands;

    public:
        /// Create commands callbacks
        Commands(Luna &lua);

        /// Destroy commands callbacks
        ~Commands();

    public:
        /// Setup callbacks functions
        void setCallbacks(SaslCommandCallbacks *callbacks,
                void *data);

        /// Find command with specified name
        SaslCommand findCommand(const char *name);

        /// Start command execution
        void commandBegin(SaslCommand command);
        
        /// Finish command execution
        void commandEnd(SaslCommand command);
        
        /// Start and finish immediatelly command execution
        void commandOnce(SaslCommand command);
        
        /// Create command with specified name and description
        SaslCommand createCommand(const char *name, const char *descr);

        /// Called by host application on command processing
        int handleCommand(SaslCommand command, int phase, 
                CommandHandler *handler);

        /// Add command handler callback
        int registerCommandHandler(lua_State *L);

        /// remove command handler callback
        int unregisterCommandHandler(lua_State *L);
};


};

#endif

