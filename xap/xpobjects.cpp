#include "xpobjects.h"


static int phase_;

#if defined(__MACH__)
#include <CoreFoundation/CoreFoundation.h>
static int ConvertPath(const char * inPath, char * outPath, int outPathMaxLen)
{
    CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, inPath ,kCFStringEncodingMacRoman);
    if (inStr == NULL)
        return -1;
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0);
    CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    if (!CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle))
        return -1;
    CFRelease(outStr);
    CFRelease(url);
    CFRelease(inStr);
    return 0;
}
#endif

#include <vector>
#include <string.h>

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

    // Parameters to control in which HDR/shadow phase objects are drawn
    int startPhase;
    int numPhases;
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

    // ugly hack
    char xpPath[1024];
    XPLMGetSystemPath(xpPath);
    char path[1024];
#if defined(__MACH__)
    int result = ConvertPath(xpPath, path, 1024);
    if (result != 0)
        return 0;
#else
    strncpy(path, xpPath, 1024);
#endif

    const char *objPath = lua_tostring(L, 1);
    if (strlen(objPath) <= strlen(path))
        return 0;
#if IBM==1 or LIN==1
    XPLMObjectRef ref = XPLMLoadObject(objPath + strlen(path));
#else
    XPLMObjectRef ref = XPLMLoadObject(objPath + strlen(path) + 1);
#endif
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
    if (9 != lua_gettop(L) && 11 != lua_gettop(L))
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
    if (11 == lua_gettop(L))
    {
        c.startPhase = lua_tonumber(L, 10);
        c.numPhases = lua_tonumber(L, 11);
    } else
    {
        c.startPhase = 0;
        c.numPhases = 999;
    }

    objectsToDraw.push_back(c);

    return 0;
}


void xap::drawObjects()
{
    if (objectsToDraw.size()) {
        for (std::vector<DrawCommand>::iterator i = objectsToDraw.begin();
                i != objectsToDraw.end(); i++)
        {
            DrawCommand &c = *i;
            // This was suggested by Ben Supnik
            if (phase_ >= c.startPhase && phase_ < c.startPhase+ c.numPhases)
                XPLMDrawObjects(c.object, 1, &c.location, c.lighting,
                                c.earthRelative);
        }

        // Note deletion happens in frameFinished() function
    }

    if (objectsToDelete.size()) {
        for (std::vector<XPLMObjectRef>::iterator i = objectsToDelete.begin();
                i != objectsToDelete.end(); i++)
            XPLMUnloadObject(*i);
        objectsToDelete.clear();
    }
    phase_++;
}

void xap::frameFinished()
{
    phase_ = 0;
    objectsToDraw.clear();
}


void xap::exportObjectsFunctions(lua_State *L)
{
    lua_register(L, "loadObjectFromFile", loadObject);
    lua_register(L, "unloadObject", unloadObject);
    lua_register(L, "drawObject", drawObject);
}

