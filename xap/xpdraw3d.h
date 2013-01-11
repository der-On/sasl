#ifndef __XPDRAW3D_H__
#define __XPDRAW3D_H__

extern "C" {
#include <lua.h>
}

#include "xpsdk.h"

namespace xap3d {

/// register X-Plane objects functions in Lua
void exportDraw3dFunctions(lua_State *L);

/// called for each stage to draw objects
void draw3d(XPLMDrawingPhase phase);

void initDraw3d();

void frameFinished();

};

#endif
