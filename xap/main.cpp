#include <string>
#include <string.h>
#include <stdio.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "main.h"
#include "xpsdk.h"
#include "xavionics.h"
#include "props.h"
#include "commands.h"
#include "gui.h"
#include "options.h"


// version of plug-in
#include "../version.h"

using namespace xap;



// plugin is enabled or disabled
static bool disabled = false;

// X-Avionics handler
XA xap::xa = NULL;

// True for 2D panels
static bool has2d = false;

/// Reference to panel left in panel coordinates
static XPLMDataRef panelLeft;

/// Reference to panel bottom in panel coordinates
static XPLMDataRef panelBottom;

/// Reference to panel left in window coordinates
//static XPLMDataRef windowLeft;
        
/// Height of screen
//static double offset2d;

/// True if panel view options were initialized
static bool panelViewInitialized = false;

/// I needn't window, but there are no other ways to detect mouse clicks
static XPLMWindowID fakeWindow;

/// Reference to mouse X coord in texture coord space
static XPLMDataRef clickX;

/// Reference to mouse Y coord in texture coord space
static XPLMDataRef clickY;

/// Reference show panel click spots property
static XPLMDataRef showClickable;

/// last value of show clickable property
static int lastShowClickable;

/// Width of 2D panel in pixels
static int panelWidth2d;

/// Height of 2D panel in pixels
static int panelHeight2d;

/// Width of 3D panel in pixels
static int panelWidth3d;

/// Height of 3D panel in pixels
static int panelHeight3d;

/// Last known width of panel
static int lastPanelWidth;

/// Last known height of panel
static int lastPanelHeight;

/// reload panel hot key command
XPLMCommandRef reloadCommand;

/// Cockpit light red component
static XPLMDataRef cockpitRed;

/// Cockpit light green component
static XPLMDataRef cockpitGreen;

/// Cockpit light blue component
static XPLMDataRef cockpitBlue;

/// Is cockpit transparent?
static XPLMDataRef cockpitTransparent;

/// Width of popup layer
static int popupWidth;

/// Height of popup layer
static int popupHeight;

/// Properties
static Props props = NULL;

/// Options storage
Options xap::options;

// visible panel region in screen coords
static XPLMDataRef panelWinB;
static XPLMDataRef panelWinT;
static XPLMDataRef panelWinL;
static XPLMDataRef panelWinR;

/// width of screen
static XPLMDataRef screenWidth;

/// height of screen
static XPLMDataRef screenHeight;

/// type of panel view
static XPLMDataRef viewType;

/// Convert a Mac-style path with double colons to a POSIX path. Google for "FSRef to POSIX path"
// if you want to know the ass-backwards way of doing it properly, but we won't link to Carbon
// only for this mmkay?
static std::string carbonPathToPosixPath(const std::string &carbonPath)
{
    // Check if we are on a Mac first, could be also #ifdef APL
    std::string sep = XPLMGetDirectorySeparator();
    if(sep != std::string(":")) return carbonPath;
    
    // Prepend the "Volumes" superdir to the path
    std::string posixPath = std::string("/Volumes/") + carbonPath;
    // Replace dots with slashes
    for(unsigned int i = 0; i < posixPath.length(); i++) {
        if(posixPath[i] == ':') posixPath[i] = '/';
    }
    XPLMDebugString("XAP: Translated path\n");
    XPLMDebugString(posixPath.c_str());
    return posixPath;
}

// Overrides XPLMGetDirectorySeparator() to return the POSIX / instead of : for paths on OS X
static std::string getDirSeparator()
{
    std::string sep = XPLMGetDirectorySeparator();
    if(sep == std::string(":")) {
        XPLMDebugString("XAP: Using Mac paths\n");
        return std::string("/");
    } else {
        return sep;
    }
}

/// Returns name of configuration file
static std::string getConfigFileName()
{
    char path[600], translatedPath[600 + 9];
    
    XPLMGetPrefsPath(path);
    XPLMExtractFileAndPath(path);
    
    strcpy(translatedPath, carbonPathToPosixPath(std::string(path)).c_str());
    strcat(translatedPath, getDirSeparator().c_str());
    strcat(translatedPath, "xap.prf");
    return translatedPath;
}

/// Returns true if file exists
static bool fileDoesExist(const std::string &path)
{
    FILE *f = fopen(path.c_str(), "r");
    if (! f)
        return false;
    fclose(f);
    return true;
}


/// Returns directory of current aircraft, with the trailing separator
static std::string getAircraftDir()
{
    char model[512], path[512];
    XPLMGetNthAircraftModel(0, model, path);
    XPLMExtractFileAndPath(path);
    
    std::string dir = carbonPathToPosixPath(std::string(path)) + getDirSeparator(); 
    
    XPLMDebugString("XAP: Got aircraft dir\n");
    XPLMDebugString(dir.c_str());
    XPLMDebugString("\nXAP: If the path does not look sane there was likely a problem with path translation\n");
    return dir;
}


/// Returns path to aircraft panel
static std::string getPanelPath(const std::string &aircraftDir)
{
    return aircraftDir + "avionics.lua";
}


void bindTexture2dCallback(int textureId)
{
    XPLMBindTexture2d(textureId, 0);
}


int genTexNameCallback()
{
    int res;
    XPLMGenerateTextureNumbers(&res, 1);
    return res;
}


/// Returns value of property as double
static double getPropd(const char *name)
{
    XPLMDataRef ref = XPLMFindDataRef(name);
    if (! ref) {
        printf("Can't find property '%s'\n", name);
        return 0.0;
    } else
        return XPLMGetDataf(ref);
}

/// Returns value of property as int
/*static int getPropi(const char *name)
{
    XPLMDataRef ref = XPLMFindDataRef(name);
    if (! ref) {
        printf("Can't find property '%s'\n", name);
        return 0;
    } else
        return XPLMGetDatai(ref);
}*/


// Calculate panel coords
static void calculatePanelSize()
{
    if (! panelHeight2d) {
        double top = getPropd("sim/graphics/view/panel_total_pnl_t");
        double bottom = getPropd("sim/graphics/view/panel_total_pnl_b");
        double panelTotalHeight = top - bottom;
        panelHeight2d = (int)panelTotalHeight;
    }
    if (! panelHeight3d)
        panelHeight3d = panelHeight2d;

    if (! panelWidth2d) {
        double left = getPropd("sim/graphics/view/panel_total_pnl_l");
        double right = getPropd("sim/graphics/view/panel_total_pnl_r");
        double panelTotalWidth = right - left;
        panelWidth2d = (int)panelTotalWidth;
    }
    if (! panelWidth3d)
        panelWidth3d = panelWidth2d;
}


static void setPanelSize(int width, int height)
{
    if ((lastPanelWidth != width) || (lastPanelHeight != height)) {
        xa_set_panel_size(xa, width, height);
        lastPanelWidth = width;
        lastPanelHeight = height;
    }
}


/// returns true if 2D panel is active
static bool is2dPanelView()
{
    return 1000 == XPLMGetDatai(viewType);
}


/// Update size of panel in libavionics
static void updatePanelSize()
{
    if ((! has2d) || (has2d && (! is2dPanelView())))
        setPanelSize(panelWidth3d, panelHeight3d);
    else 
        setPanelSize(panelWidth2d, panelHeight2d);
}


// Calculate popup coords
static void updatePopupSize()
{
    int width = XPLMGetDatai(screenWidth);
    int height = XPLMGetDatai(screenHeight);
    if ((popupWidth != width) || (popupHeight != height)) {
        xa_set_popup_size(xa, width, height);
        popupWidth = width;
        popupHeight = height;
    }
}


/// draws gauges
static int drawGauges(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    if (xa) {
        int clickable = XPLMGetDatai(showClickable);
        if (clickable != lastShowClickable) {
            xa_set_show_clickable(xa, clickable);
            lastShowClickable = clickable;
        }

        glPushMatrix();

        if (! panelViewInitialized) {
            calculatePanelSize();
            panelViewInitialized = true;
        }

        updatePanelSize();

        glTranslatef(XPLMGetDataf(panelLeft), XPLMGetDataf(panelBottom), 0);

        XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);

        xa_set_background_color(xa, XPLMGetDataf(cockpitRed), 
                XPLMGetDataf(cockpitGreen), XPLMGetDataf(cockpitBlue),
                XPLMGetDatai(cockpitTransparent) ? 0.5f : 1.0f);

        xa_draw_panel(xa, STAGE_GAUGES);
    
        glPopMatrix();
    }
    return 1;
}


/// draws popups layer
static int drawPopups(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    if (xa) {
        glPushMatrix();

        updatePopupSize();

        XPLMSetGraphicsState(0, 1, 0, 0, 1, 1, 1);
        
        xa_draw_panel(xa, STAGE_POPUPS);
    
        glPopMatrix();
    }
    return 1;
}



/// Do nothing.  It is here to keep x-plane happy
static void notDrawWindow(XPLMWindowID inWindowID, void *inRefcon)
{
}

/// Do nothing.  It is here to keep x-plane happy
static void notHandleKey(XPLMWindowID inWindowID, char inKey, 
        XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon, 
        int losingFocus)
{
}



static int handleMouseLayerClick(int x, int y, XPLMMouseStatus status, 
        int layer)
{
    switch (status) {
        case xplm_MouseDown: 
            return xa_mouse_button_down(xa, x, y, 1, layer);
        case xplm_MouseUp: 
            return xa_mouse_button_up(xa, x, y, 1, layer);
    }
    return 0;
}


/// Convert coords passed to x-plane callback to something more sensible
static void getPanelCoords(int mouseX, int mouseY, float &x, float &y)
{
    if ((! has2d) || (has2d && (! is2dPanelView()))) {
        float texX = XPLMGetDataf(clickX);
        float texY = XPLMGetDataf(clickY);
        x = texX * (float)panelWidth3d;
        y = texY * (float)panelHeight3d;
    } else {
        float l = XPLMGetDataf(panelWinL);
        float b = XPLMGetDataf(panelWinB);
        float panelTotalWidth = XPLMGetDataf(panelWinR) - l;
        float panelTotalHeight = XPLMGetDataf(panelWinT) - b;
        x = ((float)mouseX - l) / panelTotalWidth * panelWidth2d;
        y = ((float)mouseY - b) / panelTotalHeight * panelHeight2d;
    }
}


static bool clicked = false;

/// Called when user clicked on window
static int handleMouseClick(XPLMWindowID window, int panelX, int panelY,
        XPLMMouseStatus status, void *data)
{
    if (! xa)
        return 0;

    if (clicked && (xplm_MouseDrag == status))
        return 1;

    if (xplm_MouseUp == status)
        clicked = false;

    // process popup layer
    int handled = handleMouseLayerClick(panelX, panelY, status, 1);
    if (handled) {
        if (xplm_MouseDown == status)
            clicked = true;
        return 1;
    }

    // process panel layer
    float x, y;
    getPanelCoords(panelX, panelY, x, y);
    int res = handleMouseLayerClick((int)x, (int)y, status, 2);
    if (res && (xplm_MouseDown == status))
        clicked = true;
    if ((! res) && (xplm_MouseDown == status))
        handleMouseLayerClick((int)x, (int)y, xplm_MouseUp, 2);
    return res;
}

static XPLMCursorStatus handleCursor(XPLMWindowID inWindowID, int x, int y, 
        void* inRefcon)
{
    if (! xa)
        return xplm_CursorDefault;

    static int lastX = -1;
    static int lastY = -1;

    if ((x != lastX) || (y != lastY)) {
        lastX = x;
        lastY = y;
        if (! xa_mouse_move(xa, x, y, 1)) {
            float px, py;
            getPanelCoords(x, y, px, py);
            xa_mouse_move(xa, (int)px, (int)py, 2);
        }
    }

    return xplm_CursorDefault;
}

/// Creates screen-wide invisible window with mouse press handler
static XPLMWindowID createFakeWindow()
{
    int screenWidth, screenHeight;

    XPLMGetScreenSize(&screenWidth, &screenHeight);

    XPLMCreateWindow_t win;
    memset(&win, 0, sizeof(win));

    win.structSize = sizeof(win);
    win.left = 0;
    win.top = screenHeight;
    win.right = screenWidth;
    win.bottom = 0;
    win.visible = 1;
    win.drawWindowFunc = notDrawWindow;
    win.handleMouseClickFunc = handleMouseClick;
    win.handleKeyFunc = notHandleKey;
    win.handleCursorFunc = handleCursor;
    //win.handleMouseWheelFunc = notHandleMouseWheel;

    return XPLMCreateWindowEx(&win);
}


/// Returns path to xavionics data dir
static std::string getDataDir()
{
    char buf[512];
    XPLMGetSystemPath(buf);
    
    std::string sep = getDirSeparator();
    std::string path = carbonPathToPosixPath(std::string(buf));
    
    return path +  "Resources" + sep +
        "plugins" + sep + "xap" + sep + "data";
}

// pring log message
static int luaLog(lua_State *L)
{
    std::string res;
    int n = lua_gettop(L);  /* number of arguments */
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++) {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        const char *s = lua_tostring(L, -1);  /* get result */
        if (! s)
            return luaL_error(L, "tostring must return a string to print");
        if (i > 1)
           res += "\t";
        res += s;
        lua_pop(L, 1);  /* pop result */
    }

    printf("XAP: %s\n", res.c_str());
    res += "\n";
    XPLMDebugString(res.c_str());

    return 0;
}

/// Export Lua functions
static void exportToLua(lua_State *L)
{
    lua_register(L, "print", luaLog);
}


/// Destroy avionics
/// \param keepProps if true, do not destroy properties
static void freeAvionics(bool keepProps)
{
    doneGui();
    if (! xa)
        return;
    xa_done(xa);
    xa = NULL;
    if ((! keepProps) && props) {
        propsDone(props);
        props = NULL;
    }
}


/// Returns value of panel variable as boolean
static bool getGlobalPanelValue(const char *name, bool dflt)
{
    lua_State *L = xa_get_lua(xa);
    bool res = dflt;
    lua_getglobal(L, "panel");
    lua_pushstring(L, name);
    lua_rawget(L, -2);
    if (! lua_isnil(L, -1))
        res = lua_toboolean(L, -1);
    lua_pop(L, 2);
    return res;
}


/// Returns value of panel variable as int
static int getGlobalPanelValue(const char *name, int dflt)
{
    lua_State *L = xa_get_lua(xa);
    int res = dflt;
    lua_getglobal(L, "panel");
    lua_pushstring(L, name);
    lua_rawget(L, -2);
    if (! lua_isnil(L, -1))
        res = (int)lua_tonumber(L, -1);
    lua_pop(L, 2);
    return res;
}


/// destroy current panel and load new if it exists
static void reloadPanel(bool keepProps)
{
    freeAvionics(keepProps);
        
    XPLMDebugString("XAP: Reload panel signal received\n");

    lastShowClickable = -1;

    std::string dir = getAircraftDir();
    std::string panelPath = getPanelPath(dir);

    XPLMDebugString(panelPath.c_str());
    
    if (fileDoesExist(panelPath)) {
        XPLMDebugString("XAP: Loading avionics...\n");
        panelViewInitialized = false;

        std::string dataDir = dir + "/plugins/xap/data";
        if (! fileDoesExist(dataDir + "/scripts/init.lua"))
            dataDir = getDataDir();

        xa = xa_init(dataDir.c_str());
        xa_enable_click_emulator(xa, 1);
        registerCommandsApi(xa);

        if (! props)
            props = propsInit();

        options.load();

        initGui();

        if (options.isAutoStartServer()) {
            XPLMDebugString("XAP: starting server\n");
            if (xa_start_netprop_server(xa, options.getPort(), 
                        options.getSecret().c_str())) 
                XPLMDebugString("XAP: error starting server\n");
        }

        exportToLua(xa_get_lua(xa));
        xa_set_props(xa, getPropsCallbacks(), props);
        xa_set_texture2d_binder_callback(xa, bindTexture2dCallback);
        xa_set_gen_tex_name_callback(xa, genTexNameCallback);
        if (xa_load_panel(xa, panelPath.c_str())) {
            XPLMDebugString("XAP: Can't load avionics\n");
            freeAvionics(keepProps);
        } else {
            has2d = getGlobalPanelValue("panel2d", false);
            panelWidth2d = getGlobalPanelValue("panelWidth2d", 0);
            panelHeight2d = getGlobalPanelValue("panelHeight2d", 0);
            panelWidth3d = getGlobalPanelValue("panelWidth3d", 0);
            panelHeight3d = getGlobalPanelValue("panelHeight3d", 0);
            lastPanelWidth = lastPanelHeight = 0;
            popupWidth = popupHeight = 0;
    
            XPLMDebugString("XAP: Avionics loaded\n");
        }
    } else
        XPLMDebugString("XAP: Avionics not detected\n");
}


/// Called on reload aircraft panel hot key pressed
static int reloadPanelCallback(XPLMCommandRef command, int phase, void *data)
{
    if (xplm_CommandBegin == phase)
        reloadPanel(true);
    return 1;
}

static float updateAvionics(float elapsedSinceLastCall,    
                 float elapsedTimeSinceLastFlightLoop,  int counter,    
                 void *refcon)
{
    if (xa && (! disabled)) {
        
        if (clicked) {
        }

        xa_update(xa);
        return -1;
    }
    return -1;
}



// start plugin
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
    XPLMDebugString("XAP: Starting...\n");
    const char *pluginSignature = "babichev.xap";
    
#ifdef APL
    // Mac-specific: it IS possible to have XAP installed twice, once in the ACF folder
    // and once in the X-System folder. Probably due to namespace mangling on OS X
    // the system will NOT crash the app when the plugin gets loaded twice. 
    // Thus, we query the enabled plugins for the defined signatures
    // and DO NOT activate should the other plugin already be plugged in
    
    XPLM_API XPLMPluginID other_xap_id = XPLMFindPluginBySignature(pluginSignature);
    if(-1 != other_xap_id) {
        // Figure out where the other plugin came from
        char pathToAnotherCopy[256];
        XPLMGetPluginInfo(other_xap_id, NULL, pathToAnotherCopy, NULL, NULL);
        XPLMDebugString("XAP: another copy already loaded from \n");
        XPLMDebugString(pathToAnotherCopy);
        XPLMDebugString("\nXAP: will not init twice, bailing\n");
        return 0;
    }
#endif

    strcpy(outName, "SASL");
    strcpy(outSig, pluginSignature);

#ifdef SNAPSHOT
#define xstr(s) str(s)
#define str(s) #s
    sprintf(outDesc, "X-Plane scriptable avionics library plugin snapshot %i.%i.%i %s", 
            VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, xstr(SNAPSHOT));
#undef str
#undef xstr
#else
    sprintf(outDesc, "X-Plane scriptable avionics library plugin v%i.%i.%i", 
            VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
#endif

    viewType = XPLMFindDataRef("sim/graphics/view/view_type");
    //windowLeft = XPLMFindDataRef("sim/graphics/view/panel_total_win_l");
    panelLeft = XPLMFindDataRef("sim/graphics/view/panel_total_pnl_l");
    panelBottom = XPLMFindDataRef("sim/graphics/view/panel_total_pnl_b");
    
    panelWinB = XPLMFindDataRef("sim/graphics/view/panel_total_win_b");
    panelWinT = XPLMFindDataRef("sim/graphics/view/panel_total_win_t");
    panelWinL = XPLMFindDataRef("sim/graphics/view/panel_total_win_l");
    panelWinR = XPLMFindDataRef("sim/graphics/view/panel_total_win_r");
    
    screenWidth = XPLMFindDataRef("sim/graphics/view/window_width");
    screenHeight = XPLMFindDataRef("sim/graphics/view/window_height");

    clickX = XPLMFindDataRef("sim/graphics/view/click_3d_x");
    clickY = XPLMFindDataRef("sim/graphics/view/click_3d_y");
    showClickable = XPLMFindDataRef("sim/graphics/misc/show_panel_click_spots");

    cockpitRed = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_r");
    cockpitGreen = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_g");
    cockpitBlue = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_b");
    cockpitTransparent = XPLMFindDataRef("sim/graphics/settings/transparent_panel");

    return 1;
}

// clean up before die
PLUGIN_API void	XPluginStop(void)
{
    /// Clean up
}


// disable plugin
PLUGIN_API void XPluginDisable(void)
{
    XPLMUnregisterCommandHandler(reloadCommand, reloadPanelCallback, 0, NULL);
    XPLMUnregisterFlightLoopCallback(updateAvionics, NULL);
    disabled = true;
    XPLMDestroyWindow(fakeWindow);
    freeAvionics(false);
}


// enable plugin
PLUGIN_API int XPluginEnable(void)
{
    options = Options(getConfigFileName());

    disabled = false;
    if (! XPLMRegisterDrawCallback(drawGauges, xplm_Phase_Gauges, 0, NULL))
        printf("XAP: Error registering draw callback at xplm_Phase_Gauges\n");
    if (! XPLMRegisterDrawCallback(drawPopups, xplm_Phase_Window, 0, NULL))
        printf("XAP: Error registering draw callback at xplm_Phase_Window\n");
    fakeWindow = createFakeWindow();
    
    reloadCommand = XPLMCreateCommand("xap/reload", "Reload SASL avionics");
    XPLMRegisterCommandHandler(reloadCommand, reloadPanelCallback, 0, NULL);
    
    reloadPanel(false);

    XPLMRegisterFlightLoopCallback(updateAvionics, -1, NULL);

    return 1;
}


PLUGIN_API void XPluginReceiveMessage(XPLMPluginID fromWho, 
        long message, void *param)
{
    if ((XPLM_MSG_PLANE_LOADED == message) && (! param))
        reloadPanel(false);
}

