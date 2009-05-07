#include "luna.h"


using namespace xa;


/// Print arguments of function
static int luaPrint(lua_State *L) 
{
    std::string str;
    
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++) {
        const char *s;

        if (i > 1)
            str += "\t";
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        if (! s)
            return luaL_error(L, "`tostring' must return a string to `print'");
        str += s;
        lua_pop(L, 1);  /* pop result */
    }

    printf("%s\n", str.c_str());

    return 0;
}



Luna::Luna()
{
    lua = lua_open();
    luaL_openlibs(lua);
    lua_register(lua, "print", luaPrint);

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
    try {
        if (! lua_isnil(lua, -1))
            v = (float)lua_tonumber(lua, -1);
    } catch (...) { }
    lua_pop(lua, 1);
    return v;
}

double Luna::getFieldd(int tableIdx, const std::string &field, double dflt)
{
    lua_getfield(lua, tableIdx, field.c_str());
    double v = dflt;
    try {
        if (! lua_isnil(lua, -1))
            v = lua_tonumber(lua, -1);
    } catch (...) {
    }
    lua_pop(lua, 1);
    return v;
}

std::string Luna::getFields(int tableIdx, const std::string &field,
        const std::string &dflt)
{
    std::string s(dflt);
    lua_getfield(lua, tableIdx, field.c_str());
    try {
        if (! lua_isnil(lua, -1))
            s = lua_tostring(lua, -1);
    } catch (...) { }
    lua_pop(lua, 1);
    return s;
}

bool Luna::getFieldb(int tableIdx, const std::string &field, bool dflt)
{
    bool v = dflt;
    lua_getfield(lua, tableIdx, field.c_str());
    try {
        if (! lua_isnil(lua, -1))
            v = lua_toboolean(lua, -1);
    } catch (...) { }
    lua_pop(lua, 1);
    return v;
}


int Luna::getFieldi(int tableIdx, const std::string &field, int dflt)
{
    int v = dflt;
    lua_getfield(lua, tableIdx, field.c_str());
    try {
        if (! lua_isnil(lua, -1))
            v = (int)lua_tonumber(lua, -1);
    } catch (...) { }
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

