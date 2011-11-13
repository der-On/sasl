#include "avionics.h"

#include "graph.h"
#include "texture.h"
#include "libavconsts.h"
#include "propsserv.h"
#include "utils.h"
#include "graphstub.h"
#include "sound.h"


using namespace xa;


Avionics::Avionics(const std::string &path): path(path), clickEmulator(timer),
    fontManager(textureManager), properties(lua), server(log, properties), 
    commands(lua)
{
    log.exportToLua(lua);
    panelWidth = popupWidth = 1024;
    panelHeight = popupHeight = 768;
    setGraphicsCallbacks(getGraphicsStub());
    lastGcTime = 0;

    bgR = bgG = bgB = 1.0f;
    bgA = 0.0f;

    lua.storeAvionics(this);
    exportGraphToLua(lua);
    exportTextureToLua(lua);
    exportFontToLua(lua);
    exportPropsToLua(lua);
    sound.exportSoundToLua(lua);

    clickEmulation = false;
}

Avionics::~Avionics()
{
    lua_State *L = lua.getLua();
    lua_getglobal(L, "doneAvionics");        
    lua_pcall(L, 0, 0, 0);
}

int Avionics::initLua()
{
    if (lua.runScript(path + "/scripts/init.lua")) {
        log.error("Error running init script: %s", 
                lua_tostring(lua.getLua(), -1));
        lua_pop(lua.getLua(), 1);
        return -1;
    }
    addSearchPath(path + "/scripts");
    addSearchPath(path + "/components");
    addSearchImagePath(path + "/images");

    return 0;
}

void Avionics::setPanelResolution(int width, int height)
{
    panelWidth = width;
    panelHeight = height;
    
    lua_State *L = lua.getLua();
    lua_getglobal(L, "resizePanel");        
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    if (lua_pcall(L, 2, 0, 0)) {
        log.error("Can't resize panel: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}


void Avionics::setPopupResolution(int width, int height)
{
    popupWidth = width;
    popupHeight = height;
    
    lua_State *L = lua.getLua();
    lua_getglobal(L, "resizePopup");        
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    if (lua_pcall(L, 2, 0, 0)) {
        log.error("Can't resize popup layer: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}


int Avionics::loadPanel(const std::string &fileName)
{
    std::string panelDir = getDirectory(fileName);

    addSearchPath(panelDir);
    addSearchPath(panelDir + "/Custom Avionics");
    addSearchImagePath(panelDir + "/Custom Avionics/images");
    
    lua_State *L = lua.getLua();

    lua_pushstring(L, panelDir.c_str());
    lua_setglobal(L, "panelDir");

    lua_getglobal(L, "loadPanel");              // loadPanel
    lua_pushstring(L, fileName.c_str());        // loadPanel "fileName"
    lua_pushnumber(L, panelWidth);
    lua_pushnumber(L, panelHeight);
    lua_pushnumber(L, popupWidth);
    lua_pushnumber(L, popupHeight);
    if (lua_pcall(L, 5, 1, 0)) {
        const char* msg = lua_tostring(L, -1);
        std::string reason;
        if (msg)
            log.error("Error loading panel: %s", msg);
        else
            log.error("Error loading panel");
        lua_pop(L, 1);
        return -1;
    }
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return -1;
    }

    return 0;
}

void Avionics::update()
{
    if (properties.update())
        log.error("Error updating properties");

    if (server.isRunning())
        if (server.update())
            log.error("Server error");

    if (clickEmulation) {
        if (clickEmulator.update())
            onMouseClick(clickEmulator.getX(), clickEmulator.getY(),
                    clickEmulator.getButton(), clickEmulator.getLayer());
    }
    
    sound.update();

    lua_State *L = lua.getLua();
    lua_getglobal(L, "update");
    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0)) {
            std::string msg(lua_tostring(L, -1));
            lua_pop(L, 1);
            log.error("Error updating avionics: %s",  msg.c_str());
        }
    } else
        lua_pop(L, 1);

    long currentTime = timer.getTime();
    if (currentTime - lastGcTime > 3000) {
//        lua_gc(L, LUA_GCCOLLECT, 0);
        lastGcTime = currentTime;
    }
}

void Avionics::draw(int stage)
{
    lua_State *L = lua.getLua();

    graphics->draw_begin(graphics);

    const char *drawFunc;
    switch (stage) {
        case STAGE_GAUGES: drawFunc = "drawPanelLayer"; break;
        case STAGE_POPUPS: drawFunc = "drawPopupsLayer"; break;
        case STAGE_ALL: drawFunc = "drawPanel"; break;
        default: drawFunc = "";
    }

    lua_getglobal(L, drawFunc);
    if (! lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        log.error("Can't find drawPanel function");
    }

    if (lua_pcall(L, 0, 0, 0)) {
        // panel error message
        const char* msg = lua_tostring(L, -1);
        lua_pop(L, 1);
        std::string reason;
        if (msg)
            reason = std::string("Error drawing panel: ") + msg;
        else
            reason = std::string("Error drawing panel");
        log.error(reason.c_str());
    }
    
    graphics->draw_end(graphics);
}

void Avionics::addSearchPath(const std::string &path)
{
    lua_State *L = lua.getLua();
    lua_getglobal(L, "addSearchPath");      // addSearchPath
    lua_pushstring(L, path.c_str());        // addSearchPath path
    if (lua_pcall(L, 1, 0, 0)) {
        std::string msg = lua_tostring(L, -1);
        lua_pop(L, 1);
        log.error("Error adding search path: %s", msg.c_str());
    }
}

void Avionics::addSearchImagePath(const std::string &path)
{
    lua_State *L = lua.getLua();
    lua_getglobal(L, "addSearchImagePath");      // addSearchImagePath
    lua_pushstring(L, path.c_str());             // addSearchImagePath path
    if (lua_pcall(L, 1, 0, 0)) {
        std::string msg = lua_tostring(L, -1);
        lua_pop(L, 1);
        log.error("Error adding image search path: %s", msg.c_str());
    }
}

static bool call2(Luna &lua, const std::string &name, int a1, int a2, Log &log)
{
    lua_State *L = lua.getLua();
    lua_getglobal(L, name.c_str());
    lua_pushnumber(L, a1);
    lua_pushnumber(L, a2);
    if (lua_pcall(L, 2, 1, 0)) {
        std::string msg = lua_tostring(L, -1);
        lua_pop(L, 1);
        log.error("Error calling %s: %s", name.c_str(), msg.c_str());
        return false;
    }
    bool res = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return res;
}

static bool call3(Luna &lua, const std::string &name, int a1, int a2, int a3,
        Log &log)
{
    lua_State *L = lua.getLua();
    lua_getglobal(L, name.c_str());
    lua_pushnumber(L, a1);
    lua_pushnumber(L, a2);
    lua_pushnumber(L, a3);
    if (lua_pcall(L, 3, 1, 0)) {
        std::string msg = lua_tostring(L, -1);
        lua_pop(L, 1);
        log.error("Error calling %s: %s", name.c_str(), msg.c_str());
        return false;
    }
    bool res = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return res;
}

static bool call4(Luna &lua, const std::string &name, 
        int a1, int a2, int a3, int a4, Log &log)
{
    lua_State *L = lua.getLua();
    lua_getglobal(L, name.c_str());
    lua_pushnumber(L, a1);
    lua_pushnumber(L, a2);
    lua_pushnumber(L, a3);
    lua_pushnumber(L, a4);
    if (lua_pcall(L, 4, 1, 0)) {
        std::string msg = lua_tostring(L, -1);
        lua_pop(L, 1);
        log.error("Error calling %s: %s", name.c_str(), msg.c_str());
        return false;
    }
    bool res = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return res;
}


bool Avionics::onMouseUp(int x, int y, int button, int layer)
{
    if (clickEmulation)
        clickEmulator.onMouseUp();
    return call4(lua, "onMouseUp", x, y, button, layer, log);
}

bool Avionics::onMouseDown(int x, int y, int button, int layer)
{
    if (clickEmulation) {
        bool res = call4(lua, "onMouseDown", x, y, button, layer, log);
        clickEmulator.onMouseDown(button, x, y, layer);
        if (onMouseClick(x, y, button, layer) || res)
            return true;
    }
    return call4(lua, "onMouseDown", x, y, button, layer, log);
}

bool Avionics::onMouseMove(int x, int y, int layer)
{
    if (clickEmulation)
        clickEmulator.onMouseMove(x, y, layer);
    return call3(lua, "onMouseMove", x, y, layer, log);
}


bool Avionics::onMouseClick(int x, int y, int button, int layer)
{
    return call4(lua, "onMouseClick", x, y, button, layer, log);
}

void Avionics::setClickParams(int delay, int period)
{
    clickEmulator.setClickParams(delay, period);
}

void Avionics::enableClickEmulator(bool enable)
{
    clickEmulation = enable;
}

bool Avionics::onKeyUp(int charCode, int keyCode)
{
    return call2(lua, "onKeyUp", charCode, keyCode, log);
}

bool Avionics::onKeyDown(int charCode, int keyCode)
{
    return call2(lua, "onKeyDown", charCode, keyCode, log);
}

void Avionics::setBackgroundColor(float r, float g, float b, float a)
{
    bgR = r;
    bgG = g;
    bgB = b;
    bgA = a;
}

void Avionics::getBackgroundColor(float &r, float &g, float &b, float &a)
{
    r = bgR;
    g = bgG;
    b = bgB;
    a = bgA;
}

void Avionics::setShowClickable(bool show)
{
    lua_State *L = lua.getLua();
    lua_pushboolean(L, show);
    lua_setglobal(L, "showClickableAreas");
}


int Avionics::startPropsServer(int port, const std::string &secret)
{
    return server.start(secret.c_str(), port);
}


void Avionics::stopPropsServer()
{
    server.stop();
}


void Avionics::setCommandsCallbacks(SaslCommandCallbacks *callbacks, 
        void *data)
{
    commands.setCallbacks(callbacks, data);
}

void Avionics::setGraphicsCallbacks(SaslGraphicsCallbacks *callbacks)
{
    graphics = callbacks;
    textureManager.setGraphicsCallbacks(callbacks);
}


