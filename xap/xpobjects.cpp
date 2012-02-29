#include "xpobjects.h"

#include <vector>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "xpsdk.h"


using namespace xap;


// delayed draw call
struct DrawCommand
{
    // object to draw
    XPLMObjectRef object;

    // position of object
    XPLMDrawInfo_t location;

    // enable or disable lighting
    int lighting;

    // draw relative earth
    int earthRelative;
};

// list of objects to draw
static std::vector<DrawCommand> objectsToDraw;

// list of objects to destroy
static std::vector<XPLMObjectRef> objectsToDelete;


// load object from file
static int loadObject(lua_State *L)
{
    if (lua_isnil(L, 1))
        return 0;

    XPLMObjectRef ref = XPLMLoadObject(lua_tostring(L, 1));
    if (! ref)
        return 0;
    lua_pushlightuserdata(L, ref);
    return 1;
}


// delete loaded object
static int unloadObject(lua_State *L)
{
    if (lua_islightuserdata(L, 1))
        return 0;
    objectsToDelete.push_back((XPLMObjectRef)lua_touserdata(L, 1));
    return 0;
}


// draw loaded object
static int drawObject(lua_State *L)
{
    if (9 != lua_gettop(L))
        return 0;

    DrawCommand c;
    c.object = (XPLMObjectRef)lua_touserdata(L, 1);
    if (! c.object)
        return 0;

    c.location.structSize = sizeof(c.location);
    c.location.x = lua_tonumber(L, 2);
    c.location.y = lua_tonumber(L, 3);
    c.location.z = lua_tonumber(L, 4);
    c.location.pitch = lua_tonumber(L, 5);
    c.location.heading = lua_tonumber(L, 6);
    c.location.roll = lua_tonumber(L, 7);
    c.lighting = lua_tonumber(L, 8);
    c.earthRelative = lua_tonumber(L, 9);

    objectsToDraw.push_back(c);

    return 0;
}


void xap::drawObjects()
{
    if (objectsToDraw.size()) {
        for (std::vector<DrawCommand>::iterator i = objectsToDraw.begin();
                i != objectsToDraw.begin(); i++)
        {
            DrawCommand &c = *i;
            XPLMDrawObjects(c.object, 1, &c.location, c.lighting, 
                    c.earthRelative);
        }
        objectsToDraw.clear();
    }
    
    if (objectsToDelete.size()) {
        for (std::vector<XPLMObjectRef>::iterator i = objectsToDelete.begin();
                i != objectsToDelete.end(); i++)
            XPLMUnloadObject(*i);
        objectsToDelete.clear();
    }
}


void xap::exportObjectsFunctions(lua_State *L)
{
    lua_register(L, "loadObjectFromFile", loadObject);
    lua_register(L, "unloadObject", unloadObject);
    lua_register(L, "drawObject", drawObject);
}

