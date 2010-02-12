#include "xplua.h"
#include <string>
#include <stdio.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "xpsdk.h"

using namespace xap;


// pring log message
static int luaLog(lua_State *L)
{
    std::string res;
    int n = lua_gettop(L);  /* number of arguments */
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++) {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        const char *s = lua_tostring(L, -1);  /* get result */
        if (! s)
            return luaL_error(L, "tostring must return a string to print");
        if (i > 1)
           res += "\t";
        res += s;
        lua_pop(L, 1);  /* pop result */
    }

    printf("XAP: %s\n", res.c_str());
    res += "\n";
    XPLMDebugString(res.c_str());

    return 0;
}

/// reload scenery
static int reloadScenery(lua_State *L)
{
    XPLMReloadScenery();
    return 0;
}


void xap::exportLuaFunctions(lua_State *L)
{
    lua_register(L, "print", luaLog);
    lua_register(L, "reloadScenery", reloadScenery);
}


