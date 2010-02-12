#ifndef __XP_LUA_H__
#define __XP_LUA_H__

extern "C" {
#include <lua.h>
}


namespace xap {


/// register X-Plane specific functions in Lua
void exportLuaFunctions(lua_State *L);


};

#endif

