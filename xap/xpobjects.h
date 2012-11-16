#ifndef __XPOBJECTS_H__
#define __XPOBJECTS_H__

extern "C" {
#include <lua.h>
}


namespace xap {

/// register X-Plane objects functions in Lua
void exportObjectsFunctions(lua_State *L);

/// called for each stage to draw objects
void drawObjects();

void frameFinished();


};



#endif

