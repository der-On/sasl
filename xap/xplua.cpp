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


// Lua wrapper for XPLMWorldToLocal function
// takes 3 double values (lat, lon, alt)
// returns 3 double values: x, y and z
static int worldToLocal(lua_State *L)
{
    double lat, lon, alt;
    double x, y, z;

    lat = lua_tonumber(L, 1);
    lon = lua_tonumber(L, 2);
    alt = lua_tonumber(L, 3);

    XPLMWorldToLocal(lat, lon, alt, &x, &y, &z);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, z);

    return 3;
}

// Lua wrapper for XPLMLocalToWorld function
// takes 3 double values (x, y and z)
// returns 3 double values:lat, lon, alt
static int localToWorld(lua_State *L)
{
    double lat, lon, alt;
    double x, y, z;

    x = lua_tonumber(L, 1);
    y = lua_tonumber(L, 2);
    z = lua_tonumber(L, 3);

    XPLMLocalToWorld(x, y, z, &lat, &lon, &alt);

    lua_pushnumber(L, lat);
    lua_pushnumber(L, lon);
    lua_pushnumber(L, alt);

    return 3;
}



void xap::exportLuaFunctions(lua_State *L)
{
    lua_register(L, "print", luaLog);
    lua_register(L, "reloadScenery", reloadScenery);
    lua_register(L, "worldToLocal", worldToLocal);
    lua_register(L, "localToWorld", localToWorld);
}


