#ifndef __COMMANDS_H__
#define __COMMANDS_H__


struct lua_State;


/// Register commands-related Lua functions
void registerCommandsApi(lua_State *L);


/// Remove all registered commands
void unregisterAllCommands();


#endif

