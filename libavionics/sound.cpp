#include "sound.h"

#include <assert.h>

#include "avionics.h"

using namespace xa;


static int luaLoadSample(lua_State *L)
{
    int sampleId = getAvionics(L)->sampleLoad(lua_tostring(L, 1));
    lua_pushnumber(L, sampleId);
    return 1;
}


static int luaSamplePlay(lua_State *L)
{
    getAvionics(L)->samplePlay(lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}


static int luaSampleSetGain(lua_State *L)
{
    getAvionics(L)->sampleSetGain(lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}


static int luaSampleSetPitch(lua_State *L)
{
    getAvionics(L)->sampleSetPitch(lua_tonumber(L, 1), lua_tonumber(L, 2));
    return 0;
}


static int luaSampleRewind(lua_State *L)
{
    getAvionics(L)->sampleRewind(lua_tonumber(L, 1));
    return 0;
}


static int luaSampleStop(lua_State *L)
{
    getAvionics(L)->sampleStop(lua_tonumber(L, 1));
    return 0;
}


static int luaIsSamplePlaying(lua_State *L)
{
    int playing = getAvionics(L)->sampleIsPlaying(lua_tonumber(L, 1));
    lua_pushboolean(L, playing);
    return 1;
}


static int luaSetMasterGain(lua_State *L)
{
    getAvionics(L)->setMasterGain(lua_tonumber(L, 1));
    return 0;
}





void xa::exportSoundToLua(Luna &lua)
{
    lua_State *L = lua.getLua();

    lua_register(L, "loadSampleFromFile", luaLoadSample);
    lua_register(L, "playSample", luaSamplePlay);
    lua_register(L, "stopSample", luaSampleStop);
    lua_register(L, "setSampleGain", luaSampleSetGain);
    lua_register(L, "setSamplePitch", luaSampleSetPitch);
    lua_register(L, "rewindSample", luaSampleRewind);
    lua_register(L, "isSamplePlaying", luaIsSamplePlaying);
    lua_register(L, "setMasterGain", luaSetMasterGain);
}

