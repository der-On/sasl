#include "commands.h"

#include <string.h>
#include "avionics.h"


using namespace xa;



/// Find X-Plane command
static int luaFindCommand(lua_State *L)
{
    if (lua_isnil(L, 1)) {
        lua_pushnil(L);
        return 1;
    }

    Avionics *avionics = getAvionics(L);
    XaCommand cmd = avionics->getCommands().findCommand(lua_tostring(L, 1));

    if (cmd)
        lua_pushlightuserdata(L, cmd);
    else
        lua_pushnil(L);

    return 1;
}

/// press command "key"
static int luaCommandBegin(lua_State *L)
{
    if (lua_isnil(L, 1))
        return 0;

    XaCommand cmd = (XaCommand)lua_touserdata(L, 1);
    if (cmd) {
        Avionics *avionics = getAvionics(L);
        avionics->getCommands().commandBegin(cmd);
    }
    return 0;
}


/// release command "key"
static int luaCommandEnd(lua_State *L)
{
    if (lua_isnil(L, 1))
        return 0;

    XaCommand cmd = (XaCommand)lua_touserdata(L, 1);
    if (cmd) {
        Avionics *avionics = getAvionics(L);
        avionics->getCommands().commandEnd(cmd);
    }
    return 0;
}


/// press and release command "key" immediatelly
static int luaCommandOnce(lua_State *L)
{
    if (lua_isnil(L, 1))
        return 0;

    XaCommand cmd = (XaCommand)lua_touserdata(L, 1);
    if (cmd) {
        Avionics *avionics = getAvionics(L);
        avionics->getCommands().commandOnce(cmd);
    }
    return 0;
}


/// Create new command
static int luaCreateCommand(lua_State *L)
{
    if (lua_isnil(L, 1) || lua_isnil(L, 2)) {
        lua_pushnil(L);
        return 1;
    }
    
    Avionics *avionics = getAvionics(L);
    XaCommand cmd = avionics->getCommands().createCommand(lua_tostring(L, 1), 
            lua_tostring(L, 2));
    if (cmd)
        lua_pushlightuserdata(L, cmd);
    else
        lua_pushnil(L);

    return 1;
}


/// handle command
static int luaCommandHandler(XaCommand command, int phase,
        void *data)
{
    Commands::CommandHandler *handler = (Commands::CommandHandler*)data;
    if (! handler)
        return 1;

    return handler->commands->handleCommand(command, phase, handler);
}


/// register Lua command handler
static int luaRegisterCommandHandler(lua_State *L)
{
    if (! (lua_islightuserdata(L, 1)) && lua_isnumber(L, 2) && 
            lua_isfunction(L, 3)) 
        return 0;

    return getAvionics(L)->getCommands().registerCommandHandler(L);
}


/// unregister Lua command handler
static int luaUnregisterCommandHandler(lua_State *L)
{
    if (! (lua_islightuserdata(L, 1)) && lua_isnumber(L, 2))
        return 0;
    
    return getAvionics(L)->getCommands().unregisterCommandHandler(L);
}



Commands::Commands(Luna &lua): lua(lua)
{
    memset(&callbacks, 0, sizeof(callbacks));
    data = NULL;
    
    lua_State *L = lua.getLua();
    lua_register(L, "findCommand", luaFindCommand);
    lua_register(L, "commandBegin", luaCommandBegin);
    lua_register(L, "commandEnd", luaCommandEnd);
    lua_register(L, "commandOnce", luaCommandOnce);
    lua_register(L, "createCommand", luaCreateCommand);
    lua_register(L, "registerCommandHandler", luaRegisterCommandHandler);
    lua_register(L, "unregisterCommandHandler", luaUnregisterCommandHandler);
}


Commands::~Commands()
{
    if (callbacks.remove_command_handler)
        for (std::list<CommandHandler>::iterator i = commands.begin(); 
                i != commands.end(); i++)
        {
            CommandHandler &h = *i;
            callbacks.remove_command_handler(h.command, luaCommandHandler,
                h.before, &h, data);
        }
}


void Commands::setCallbacks(XaCommandCallbacks *cb, void *dta)
{
    if (! cb)
        memset(&callbacks, 0, sizeof(callbacks));
    else
        memcpy(&callbacks, cb, sizeof(callbacks));
    data = dta;
}


XaCommand Commands::findCommand(const char *name)
{
    if (callbacks.find_command)
        return callbacks.find_command(name, data);
    else
        return NULL;
}


void Commands::commandBegin(XaCommand command)
{
    if (callbacks.command_begin)
        callbacks.command_begin(command, data);
}


void Commands::commandEnd(XaCommand command)
{
    if (callbacks.command_end)
        callbacks.command_end(command, data);
}


void Commands::commandOnce(XaCommand command)
{
    if (callbacks.command_once)
        callbacks.command_once(command, data);
    else {
        commandBegin(command);
        commandEnd(command);
    }
}


XaCommand Commands::createCommand(const char *name, const char *descr)
{
    if (callbacks.create_command)
        return callbacks.create_command(name, descr, data);
    else
        return NULL;
}


int Commands::handleCommand(XaCommand command, int phase,
        CommandHandler *handler)
{
    lua_State *L = lua.getLua();

    lua.getRef(handler->callbackRef);
    lua_pushinteger(L, phase);
    
    int res = 1;
    if (lua_pcall(L, 1, 1, 0))
        printf("Error calling command handler: %s\n", lua_tostring(L, -1));
    else {
        res = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    
    return res;
}


int Commands::registerCommandHandler(lua_State *L)
{
    CommandHandler handler;
    handler.command = (XaCommand)lua_touserdata(L, 1);
    handler.commands = this;
    handler.before = lua_tointeger(L, 2);

    lua_pushvalue(L, 3);
    handler.callbackRef = lua.addRef();

    commands.push_back(handler);

    if (callbacks.add_command_handler)
        callbacks.add_command_handler(handler.command, luaCommandHandler,
            handler.before, &(commands.back()), data);

    return 0;
}


int Commands::unregisterCommandHandler(lua_State *L)
{
    XaCommand command = (XaCommand)lua_touserdata(L, 1);
    int before = lua_tointeger(L, 2);

    CommandHandler *handler = NULL;

    for (std::list<CommandHandler>::iterator i = commands.begin(); 
            i != commands.end(); i++)
    {
        CommandHandler &h = *i;
        if ((h.command == command) && (h.before == before)) {
            lua.unRef(h.callbackRef);

            handler = &h;
            commands.erase(i);
            break;
        }
    }

    if (callbacks.remove_command_handler && handler)
        callbacks.remove_command_handler(handler->command, luaCommandHandler,
            handler->before, handler, data);

    return 0;
}

