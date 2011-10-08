#include "luna.h"

#include <sys/types.h>
#include <dirent.h>


using namespace xa;


/// bitwise and
static int luaBitAnd(lua_State *L) 
{
    int n = lua_gettop(L);
    int res = 0;
    if (2 == n) 
        res = lua_tointeger(L, 1) & lua_tointeger(L, 2);
    lua_pushinteger(L, res);
    return 1;
}

/// bitwise or
static int luaBitOr(lua_State *L) 
{
    int n = lua_gettop(L);
    int res = 0;
    if (2 == n) 
        res = lua_tointeger(L, 1) | lua_tointeger(L, 2);
    lua_pushinteger(L, res);
    return 1;
}

/// bitwise xor
static int luaBitXor(lua_State *L) 
{
    int n = lua_gettop(L);
    int res = 0;
    if (2 == n) 
        res = lua_tointeger(L, 1) ^ lua_tointeger(L, 2);
    lua_pushinteger(L, res);
    return 1;
}

/// enumerate files in directory
static int luaListFiles(lua_State *L) 
{
    const char *name = lua_tostring(L, 1);
    if (! name)
        return 0;

    lua_newtable(L);

    DIR *dir = opendir(name);
    if (! dir) {
        return 1;
    }

    int i = 1;
    for (struct dirent *de = readdir(dir); de; de = readdir(dir)) {
        if ((DT_DIR == de->d_type) || (DT_LNK == de->d_type) || 
                (DT_REG == de->d_type))
        {
            lua_pushnumber(L, i);
            lua_newtable(L);
            lua_pushstring(L, "name");
            lua_pushstring(L, de->d_name);
            lua_settable(L, -3);
            lua_pushstring(L, "type");
            lua_pushstring(L, DT_DIR == de->d_type ? "dir" : "file");
            lua_settable(L, -3);
            lua_settable(L, -3);
            i++;
        }
    }
    closedir(dir);

    return 1;
}


Luna::Luna()
{
    lua = luaL_newstate();
    luaL_openlibs(lua);
    lua_register(lua, "bitand", luaBitAnd);
    lua_register(lua, "bitor", luaBitOr);
    lua_register(lua, "bitxor", luaBitXor);
    lua_register(lua, "listFiles", luaListFiles);

    lua_newtable(lua);
    lua_setfield(lua, LUA_REGISTRYINDEX, "xavionics");
}

Luna::~Luna()
{
    lua_close(lua);
}

bool Luna::runScript(const std::string &fileName)
{
    return luaL_dofile(lua, fileName.c_str());
}

float Luna::getFieldf(int tableIdx, const std::string &field, float dflt)
{
    float v = dflt;
    lua_getfield(lua, tableIdx, field.c_str());
    if (! lua_isnil(lua, -1))
        v = (float)lua_tonumber(lua, -1);
    lua_pop(lua, 1);
    return v;
}

double Luna::getFieldd(int tableIdx, const std::string &field, double dflt)
{
    lua_getfield(lua, tableIdx, field.c_str());
    double v = dflt;
    if (! lua_isnil(lua, -1))
        v = lua_tonumber(lua, -1);
    lua_pop(lua, 1);
    return v;
}

std::string Luna::getFields(int tableIdx, const std::string &field,
        const std::string &dflt)
{
    std::string s(dflt);
    lua_getfield(lua, tableIdx, field.c_str());
    if (! lua_isnil(lua, -1))
        s = lua_tostring(lua, -1);
    lua_pop(lua, 1);
    return s;
}

bool Luna::getFieldb(int tableIdx, const std::string &field, bool dflt)
{
    bool v = dflt;
    lua_getfield(lua, tableIdx, field.c_str());
    if (! lua_isnil(lua, -1))
        v = lua_toboolean(lua, -1);
    lua_pop(lua, 1);
    return v;
}


int Luna::getFieldi(int tableIdx, const std::string &field, int dflt)
{
    int v = dflt;
    lua_getfield(lua, tableIdx, field.c_str());
    if (! lua_isnil(lua, -1))
        v = (int)lua_tonumber(lua, -1);
    lua_pop(lua, 1);
    return v;
}


int Luna::addRef()
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "xavionics");
    lua_pushvalue(lua, -2);
    int ref = luaL_ref(lua, -2);
    lua_pop(lua, 2);
    return ref;
}

void Luna::getRef(int ref)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "xavionics");
    lua_rawgeti(lua, -1, ref);
    lua_remove(lua, -2);
}

void Luna::unRef(int ref)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "xavionics");
    luaL_unref(lua, -1, ref);
}

void Luna::storeAvionics(Avionics *avionics)
{
    lua_pushlightuserdata(lua, avionics);
    lua_setfield(lua, LUA_REGISTRYINDEX, "avionics");
}

Avionics* xa::getAvionics(lua_State *lua)
{
    lua_getfield(lua, LUA_REGISTRYINDEX, "avionics");
    Avionics *v = (Avionics*)lua_touserdata(lua, -1);
    lua_pop(lua, 1);
    return v;
}

