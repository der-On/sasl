#ifndef __XP_LUA_H__
#define __XP_LUA_H__

extern "C" {
#include <lua.h>
}


namespace xap {


/// register X-Plane specific functions in Lua
void exportLuaFunctions(lua_State *L);

/// deallocate allocated objects
void doneLuaFunctions();

// create new Lua state with custom allocator
lua_State* luaCreatorCallback();

// destroy lua state with external allocator
void luaDestroyerCallback(lua_State *lua);

};

#endif

