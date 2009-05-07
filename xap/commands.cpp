#include "commands.h"
#include "xpsdk.h"


extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <list>


/// Find X-Plane command
static int luaFindCommand(lua_State *L)
{
    if (lua_isnil(L, 1)) {
        lua_pushnil(L);
        return 1;
    }

    XPLMCommandRef cmd = XPLMFindCommand(lua_tostring(L, 1));
    if (cmd)
        lua_pushlightuserdata(L, cmd);
    else
        lua_pushnil(L);

    return 1;
}

/// press command "key"
static int luaCommandBegin(lua_State *L)
{
    XPLMCommandRef cmd = (XPLMCommandRef)lua_touserdata(L, 1);
    if (cmd)
        XPLMCommandBegin(cmd);
    return 0;
}


/// release command "key"
static int luaCommandEnd(lua_State *L)
{
    XPLMCommandRef cmd = (XPLMCommandRef)lua_touserdata(L, 1);
    if (cmd)
        XPLMCommandEnd(cmd);
    return 0;
}


/// press and release command "key" immediatelly
static int luaCommandOnce(lua_State *L)
{
    XPLMCommandRef cmd = (XPLMCommandRef)lua_touserdata(L, 1);
    if (cmd)
        XPLMCommandOnce(cmd);
    return 0;
}


/// Create new command
static int luaCreateCommand(lua_State *L)
{
    if (lua_isnil(L, 1) || lua_isnil(L, 2)) {
        lua_pushnil(L);
        return 1;
    }
    
    XPLMCommandRef cmd = XPLMCreateCommand(lua_tostring(L, 1), 
            lua_tostring(L, 2));
    if (cmd)
        lua_pushlightuserdata(L, cmd);
    else
        lua_pushnil(L);

    return 1;
}


/// Lua command handler description
struct CommandHandler
{
    /// command to handle
    XPLMCommandRef command;

    /// Lua state
    lua_State *lua;

    /// When to call: before or after default handler
    int before;

    /// reference to callback
    int callbackRef;
};


/// Lif of registered commands
static std::list<CommandHandler> commands;


/// handle command
static int luaCommandHandler(XPLMCommandRef command, XPLMCommandPhase phase,
        void *refcon)
{
    CommandHandler *handler = (CommandHandler*)refcon;
    if (! handler)
        return 0;

    lua_State *L = handler->lua;

    lua_getfield(L, LUA_REGISTRYINDEX, "xapcommands");
    lua_rawgeti(L, -1, handler->callbackRef);
    lua_remove(L, -2);
    lua_pushinteger(L, phase);
    
    int res = 1;
    if (lua_pcall(L, 1, 1, 0))
        printf("Error calling command handler: %s\n", lua_tostring(L, -1));
    else
        res = lua_tointeger(L, -1);
    
    lua_pop(L, 1);

    return res;
}


/// register Lua command handler
static int luaRegisterCommandHandler(lua_State *L)
{
    if (! (lua_islightuserdata(L, 1)) && lua_isnumber(L, 2) && 
            lua_isfunction(L, 3)) 
        return 0;

    CommandHandler handler;
    handler.command = (XPLMCommandRef)lua_touserdata(L, 1);
    handler.lua = L;
    handler.before = lua_tointeger(L, 2);

    lua_getfield(L, LUA_REGISTRYINDEX, "xapcommands");
    lua_pushvalue(L, 3);
    handler.callbackRef = luaL_ref(L, -2);
    lua_pop(L, 2);

    commands.push_back(handler);

    XPLMRegisterCommandHandler(handler.command, luaCommandHandler,
            handler.before, &(commands.back()));

    return 0;
}


/// unregister Lua command handler
static int luaUnregisterCommandHandler(lua_State *L)
{
    if (! (lua_islightuserdata(L, 1)) && lua_isnumber(L, 2))
        return 0;

    XPLMCommandRef *command = (XPLMCommandRef*)lua_touserdata(L, 1);
    int before = lua_tointeger(L, 2);

    CommandHandler *handler = NULL;

    for (std::list<CommandHandler>::iterator i = commands.begin(); 
            i != commands.end(); i++)
    {
        CommandHandler &h = *i;
        if ((h.command == command) && (h.before == before)) {
            lua_getfield(L, LUA_REGISTRYINDEX, "xapcommands");
            luaL_unref(L, -1, h.callbackRef);
            lua_pop(L, 1);

            handler = &h;
            commands.erase(i);
            break;
        }
    }

    XPLMUnregisterCommandHandler(command, luaCommandHandler, before, handler);

    return 0;
}


void registerCommandsApi(lua_State *L)
{
    lua_register(L, "findCommand", luaFindCommand);
    lua_register(L, "commandBegin", luaCommandBegin);
    lua_register(L, "commandEnd", luaCommandEnd);
    lua_register(L, "commandOnce", luaCommandOnce);
    lua_register(L, "createCommand", luaCreateCommand);
    lua_register(L, "registerCommandHandler", luaRegisterCommandHandler);
    lua_register(L, "unregisterCommandHandler", luaUnregisterCommandHandler);
    
    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, "xapcommands");
}


void unregisterAllCommands()
{
    for (std::list<CommandHandler>::iterator i = commands.begin(); 
            i != commands.end(); i++)
    {
        CommandHandler &h = *i;
        XPLMUnregisterCommandHandler(h.command, luaCommandHandler, 
                h.before, &h);
        lua_getfield(h.lua, LUA_REGISTRYINDEX, "xapcommands");
        luaL_unref(h.lua, -1, h.callbackRef);
        lua_pop(h.lua, 1);
    }

    commands.clear();
}

