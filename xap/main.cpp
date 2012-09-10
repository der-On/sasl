#include <string>
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifndef WINDOWS
#include <alloca.h>
#else
#include <windows.h>
#endif

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>

    #ifdef APL
        // for path translation
        #include <CoreFoundation/CFString.h>
        #include <CoreFoundation/CFURL.h>
    #endif
}

#include "main.h"
#include "xpsdk.h"
#include "libavionics.h"
#include "props.h"
#include "commands.h"
#include "gui.h"
#include "options.h"
#include "xplua.h"
#include "ogl.h"
#include "alsound.h"
#include "listener.h"
#include "xpobjects.h"



// version of plug-in
#include "../version.h"

using namespace xap;

// plugin is enabled or disabled
static bool disabled = false;

// X-Avionics handler
SASL xap::sasl = NULL;

// True for 2D panels
static bool has2d = false;

/// 0 for ATTR_COCKPIT , 1 for ATTR_COCKPIT_REGION albedo, 2 for ATTR_COCKPIT_REGION emissive
static XPLMDataRef panelRenderPass;

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
static XPLMCommandRef reloadCommand;

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
static SaslProps props = NULL;

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

// view camera orientation
static XPLMDataRef viewX;
static XPLMDataRef viewY;
static XPLMDataRef viewZ;
static XPLMDataRef viewPitch;
static XPLMDataRef viewRoll;
static XPLMDataRef viewHeading;

// last mouse position
static int lastMouseX = -1;
static int lastMouseY = -1;
static bool lastMouseHandled = false;
static float lastPanelX = -1;
static float lastPanelY = -1;

// disable processing of clicks in 3D panel
static bool disablePanelClicks = false;

// OpenGL graphics functions
static SaslGraphicsCallbacks* graphics = NULL;

// sound functions
static struct SaslAlSound* sound = NULL;


/// Write message to X-Plane log
static void printToLog(int level, const char *message)
{
    if (! message)
        return;

    char levelStr[10];

    switch (level) {
        case LOG_DEBUG: sprintf(levelStr, "DEBUG"); break;
        case LOG_INFO: sprintf(levelStr, "INFO"); break;
        case LOG_WARNING: sprintf(levelStr, "WARNING"); break;
        default: sprintf(levelStr, "ERROR");
    }

    int msgLen = 20 + strlen(levelStr) + strlen(message);
    char *buf = (char*)alloca(msgLen);
    sprintf(buf, "SASL %s: %s\n", levelStr, message);
    XPLMDebugString(buf);
    printf("%s", buf);
}

/// Convert a Mac-style path with double colons to a POSIX path.
// Since Carbon is not available on other paltfornms we can safely bypass this
static std::string carbonPathToPosixPath(const std::string &carbonPath)
{
#if APL==1
    char outPathBuf[PATH_MAX]; //gothic
    CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, carbonPath.c_str() ,kCFStringEncodingMacRoman);
    if (inStr == NULL)
        return "";
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0);
    CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    if (!CFStringGetCString(outStr, outPathBuf, PATH_MAX, kCFURLPOSIXPathStyle))
        return "";
    CFRelease(outStr);
    CFRelease(url);
    CFRelease(inStr);

    return std::string(outPathBuf);

#else
    return carbonPath;
#endif
}

// Overrides XPLMGetDirectorySeparator() to return the POSIX / instead of : for paths on OS X
static std::string getDirSeparator()
{
    std::string sep = XPLMGetDirectorySeparator();
    if(sep == std::string(":")) {
        XPLMDebugString("SASL: Using Mac paths\n");
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
    strcat(translatedPath, "sasl.prf");
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
        XPLMDebugString("SASL: Can't find property ");
        XPLMDebugString(name);
        XPLMDebugString("\n");
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
        sasl_set_panel_size(sasl, width, height);
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
    if (! panelViewInitialized) {
        calculatePanelSize();
        panelViewInitialized = true;
    }

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
        sasl_set_popup_size(sasl, width, height);
        popupWidth = width;
        popupHeight = height;
    }
}


/// draws gauges
static int drawGauges(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    if (sasl && XPLMGetDatai(panelRenderPass) == 0) {
        updatePanelSize();

        glPushMatrix();

        glTranslatef(XPLMGetDataf(panelLeft), XPLMGetDataf(panelBottom), 0);
        XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
        sasl_draw_panel(sasl, STAGE_GAUGES);

        glPopMatrix();
    }

    return 1;
}


/// draws popups layer
static int drawPopups(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    if (sasl) {
        updatePopupSize();
        XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
        sasl_draw_panel(sasl, STAGE_POPUPS);
    }

    return 1;
}


// draw objects
static int drawScene(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    drawObjects();
    return 1;
}

// reset draw phase
static int drawLast2d(XPLMDrawingPhase phase, int isBefore, void *refcon)
{
    frameFinished();
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
            return sasl_mouse_button_down(sasl, x, y, 1, layer);
        case xplm_MouseUp:
            return sasl_mouse_button_up(sasl, x, y, 1, layer);
    }
    return 0;
}


// handle keyboard events
static int handleKeyboardEvent(char charCode, XPLMKeyFlags flags,
        char keyCode, void *refcon)
{
    if (! sasl)
        return 0;

    if (flags & xplm_DownFlag)
        return ! sasl_key_down(sasl, charCode, keyCode);
    if (flags & xplm_UpFlag)
        return ! sasl_key_up(sasl, charCode, keyCode);
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
    if (! sasl)
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
    } else
        if (disablePanelClicks) {
            handleMouseLayerClick(panelX, panelY, xplm_MouseUp, 1);
            return 0;
        }

    // process panel layer
    if (! disablePanelClicks) {
        float x, y;
        getPanelCoords(panelX, panelY, x, y);
        int res = handleMouseLayerClick((int)x, (int)y, status, 2);
        if (res && (xplm_MouseDown == status))
            clicked = true;
        if ((! res) && (xplm_MouseDown == status))
            handleMouseLayerClick((int)x, (int)y, xplm_MouseUp, 2);
        return res;
    } else
        return 0;
}

static XPLMCursorStatus handleCursor(XPLMWindowID inWindowID, int x, int y,
        void* inRefcon)
{
    if (! sasl)
        return xplm_CursorDefault;

    if ((x != lastMouseX) || (y != lastMouseY)) {
        lastMouseX = x;
        lastMouseY = y;
        lastMouseHandled = sasl_mouse_move(sasl, x, y, 1);
        if (! lastMouseHandled) {
            float px, py;
            getPanelCoords(x, y, px, py);
            if ((px == lastPanelX) && (py == lastPanelY)) {
                // 2d coords changed but panel position doesn't
                // looks like mouse out of panel and x-plane bug in action
                // let's move mouse to -1 -1 and hope it will be ok
                sasl_mouse_move(sasl, -1, -1, 2);
                disablePanelClicks = true;
            } else {
                disablePanelClicks = false;
                sasl_mouse_move(sasl, (int)px, (int)py, 2);
                lastPanelX = px;
                lastPanelY = py;
            }
        }
    }


    if (! lastMouseHandled)
        return xplm_CursorDefault;
    else
        return xplm_CursorArrow;
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


/// Returns path to libavionics data dir
static std::string getDataDir()
{
    char buf[512];
    XPLMGetSystemPath(buf);

    std::string sep = getDirSeparator();
    std::string path = carbonPathToPosixPath(std::string(buf));

    return path +  "Resources" + sep +
        "plugins" + sep + "sasl" + sep + "data";
}


/// Destroy avionics
/// \param keepProps if true, do not destroy properties
static void freeAvionics(bool keepProps)
{
    doneGui();
    if (! sasl)
        return;
    sasl_done_al_sound(sound);
    sound = NULL;
    sasl_done(sasl);
    sasl = NULL;
    if (props) {
        if (! keepProps) {
            propsDone(props);
            props = NULL;
        } else {
            funcPropsDone(props);
        }
    }
    doneLuaFunctions();
}


/// Returns value of panel variable as boolean
static bool getGlobalPanelValue(const char *name, bool dflt)
{
    lua_State *L = sasl_get_lua(sasl);
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
    lua_State *L = sasl_get_lua(sasl);
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
void xap::reloadPanel(bool keepProps)
{
    freeAvionics(keepProps);

    lastShowClickable = -1;

    std::string dir = getAircraftDir();
    std::string panelPath = getPanelPath(dir);

    XPLMDebugString("SASL: Path to panel: ");
    XPLMDebugString(panelPath.c_str());
    XPLMDebugString("\n");

    XPLMDebugString("SASL: Loading avionics...\n");
    panelViewInitialized = false;

    std::string dataDir = dir + "/plugins/sasl/data";
    if (! fileDoesExist(dataDir + "/scripts/init.lua"))
    dataDir = getDataDir();

    sasl = sasl_init(dataDir.c_str());
    if (! sasl) {
        XPLMDebugString("SASL: error initializing from ");
        XPLMDebugString(dataDir.c_str());
        XPLMDebugString("\n");
        return;
    }
    sasl_set_log_callback(sasl, printToLog, NULL);

    if (fileDoesExist(panelPath)) {
        sasl_enable_click_emulator(sasl, 1);
        sasl_set_graphics_callbacks(sasl, graphics);
        sound = sasl_init_al_sound(sasl);
        registerCommandsApi(sasl);

        if (! props)
            props = propsInit();

        options.load();

        initGui();

        if (options.isAutoStartServer()) {
            sasl_log_info(sasl, "Starting server");
            if (sasl_start_netprop_server(sasl, options.getPort(),
                        options.getSecret().c_str()))
                sasl_log_error(sasl, "Can't start server");
        }

        exportLuaFunctions(sasl_get_lua(sasl));
        sasl_set_props(sasl, getPropsCallbacks(), props);
        if (sasl_load_panel(sasl, panelPath.c_str())) {
            sasl_log_error(sasl, "Can't load avionics");
            freeAvionics(keepProps);
        } else {
            has2d = getGlobalPanelValue("panel2d", false);
            panelWidth2d = getGlobalPanelValue("panelWidth2d", 0);
            panelHeight2d = getGlobalPanelValue("panelHeight2d", 0);
            panelWidth3d = getGlobalPanelValue("panelWidth3d", 0);
            panelHeight3d = getGlobalPanelValue("panelHeight3d", 0);
            lastPanelWidth = lastPanelHeight = 0;
            popupWidth = popupHeight = 0;

            sasl_log_info(sasl, "Avionics loaded");
        }
    } else {
        freeAvionics(keepProps);
        XPLMDebugString("SASL: Avionics not detected\n");
    }
}


/// Called on reload aircraft panel hot key pressed
static int reloadPanelCallback(XPLMCommandRef command, int phase, void *data)
{
    if (xplm_CommandBegin == phase)
        reloadPanel(true);
    return 1;
}


// returns true if view changed since last function call
static bool isViewTheSame()
{
    static float lastViewX = -1;
    static float lastViewY = -1;
    static float lastViewZ = -1;
    static float lastViewPitch = -1;
    static float lastViewRoll = -1;
    static float lastViewHeading = -1;
    static int lastViewType = -1;

    bool same = true;

    if ((fabs(lastViewX - XPLMGetDataf(viewX)) > 0.01) ||
            (fabs(lastViewY - XPLMGetDataf(viewY)) > 0.01) ||
            (fabs(lastViewZ - XPLMGetDataf(viewZ)) > 0.01) ||
            (fabs(lastViewPitch - XPLMGetDataf(viewPitch)) > 0.01) ||
            (fabs(lastViewRoll - XPLMGetDataf(viewRoll)) > 0.01) ||
            (fabs(lastViewHeading - XPLMGetDataf(viewHeading)) > 0.01) ||
            (lastViewType != XPLMGetDatai(viewType)))
    {
        same = false;
    }

    lastViewX = XPLMGetDataf(viewX);
    lastViewY = XPLMGetDataf(viewY);
    lastViewZ = XPLMGetDataf(viewZ);
    lastViewPitch = XPLMGetDataf(viewPitch);
    lastViewRoll = XPLMGetDataf(viewRoll);
    lastViewHeading = XPLMGetDataf(viewHeading);
    lastViewType = XPLMGetDatai(viewType);

    return same;
}


static float updateAvionics(float elapsedSinceLastCall,
                 float elapsedTimeSinceLastFlightLoop,  int counter,
                 void *refcon)
{
    if (fakeWindow == 0)
    {
         fakeWindow = createFakeWindow();
    }
    if (sasl && (! disabled)) {
        int clickable = XPLMGetDatai(showClickable);
        if (clickable != lastShowClickable) {
            sasl_set_show_clickable(sasl, clickable);
            lastShowClickable = clickable;
        }

        sasl_set_background_color(sasl, XPLMGetDataf(cockpitRed),
                XPLMGetDataf(cockpitGreen), XPLMGetDataf(cockpitBlue),
                XPLMGetDatai(cockpitTransparent) ? 0.5f : 1.0f);

        // if camera position changed make 'virtual' mouse move to reset
        // cursor shape
        if (! isViewTheSame()) {
            lastMouseX = -10;
            lastMouseY = -10;
            handleCursor(fakeWindow, -1, -1, NULL);
        }
        updateListenerPosition(sasl);
        sasl_update(sasl);
    }

    return -1;
}


// call lua callback if exists
static void callCallback(const char *name)
{
    if (! sasl)
        return;

    lua_State *L = sasl_get_lua(sasl);
    lua_getglobal(L, name);
    lua_pcall(L, 0, 0, 0);
}


// start plugin
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
    XPLMDebugString("SASL: Starting...\n");
    const char *pluginSignature = "1-sim.sasl";

    // Mac-specific: it IS possible to have SASL installed twice, once in the ACF folder
    // and once in the X-System folder. Probably due to namespace mangling on OS X
    // the system will NOT crash the app when the plugin gets loaded twice.
    // Thus, we query the enabled plugins for the defined signatures
    // and DO NOT activate should the other plugin already be plugged in

    XPLMPluginID other_sasl_id = XPLMFindPluginBySignature(pluginSignature);
    if(-1 != other_sasl_id) {
        // Figure out where the other plugin came from
        char pathToAnotherCopy[256];
        XPLMGetPluginInfo(other_sasl_id, NULL, pathToAnotherCopy, NULL, NULL);
        XPLMDebugString("SASL: Another copy already loaded from ");
        XPLMDebugString(pathToAnotherCopy);
        XPLMDebugString("\nSASL: Will not init twice, bailing\n");
        return 0;
    }

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
    XPLMDebugString(outDesc);
    XPLMDebugString("\n");

    panelRenderPass =  XPLMFindDataRef("sim/graphics/view/panel_render_type");
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

    viewX = XPLMFindDataRef("sim/graphics/view/pilots_head_x");
    viewY = XPLMFindDataRef("sim/graphics/view/pilots_head_y");
    viewZ = XPLMFindDataRef("sim/graphics/view/pilots_head_z");
    viewPitch = XPLMFindDataRef("sim/graphics/view/pilots_head_the");
    viewRoll = XPLMFindDataRef("sim/graphics/view/pilots_head_psi");
    viewHeading = XPLMFindDataRef("sim/graphics/view/cockpit_heading");
    viewType = XPLMFindDataRef("sim/graphics/view/view_type");

    graphics = saslgl_init_graphics();
    saslgl_set_texture2d_binder_callback(graphics, bindTexture2dCallback);
    saslgl_set_gen_tex_name_callback(graphics, genTexNameCallback);

    initListenerTracking();

    return 1;
}

// clean up before die
PLUGIN_API void	XPluginStop(void)
{
    saslgl_done_graphics(graphics);
    graphics = NULL;
}


// disable plugin
PLUGIN_API void XPluginDisable(void)
{
    XPLMUnregisterCommandHandler(reloadCommand, reloadPanelCallback, 0, NULL);
    XPLMUnregisterFlightLoopCallback(updateAvionics, NULL);
    XPLMUnregisterKeySniffer(handleKeyboardEvent, 0, NULL);
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
        XPLMDebugString("SASL: Error registering draw callback at xplm_Phase_Gauges\n");
    if (! XPLMRegisterDrawCallback(drawPopups, xplm_Phase_Window, 0, NULL))
        XPLMDebugString("SASL: Error registering draw callback at xplm_Phase_Window\n");
    if (! XPLMRegisterDrawCallback(drawScene, xplm_Phase_Objects, 0, NULL))
        XPLMDebugString("SASL: Error registering draw callback at xplm_Phase_Objects\n");
    if (! XPLMRegisterDrawCallback(drawLast2d, xplm_Phase_Window, 0, NULL))
        XPLMDebugString("SASL: Error registering draw callback at xplm_Phase_Window (last 2d)t\n");
    fakeWindow = 0;

    reloadCommand = XPLMCreateCommand("sasl/reload", "Reload SASL avionics");
    XPLMRegisterCommandHandler(reloadCommand, reloadPanelCallback, 0, NULL);

    XPLMRegisterKeySniffer(handleKeyboardEvent, 0, NULL);

    reloadPanel(false);

    XPLMRegisterFlightLoopCallback(updateAvionics, -1, NULL);

    return 1;
}


PLUGIN_API void XPluginReceiveMessage(XPLMPluginID fromWho,
        long message, void *param)
{
    if ((XPLM_MSG_PLANE_CRASHED == message) ||
            ((XPLM_MSG_PLANE_LOADED == message) && (! param)))
        reloadPanel(false);
    else {
        if (! sasl)
            return;
        switch (message) {
            case XPLM_MSG_AIRPORT_LOADED:
                callCallback("onAirportLoaded");
                break;
            case XPLM_MSG_SCENERY_LOADED:
                callCallback("onSceneryLoaded");
                break;
            case XPLM_MSG_AIRPLANE_COUNT_CHANGED:
                callCallback("onAirplaneCountChanged");
                break;
        }
    }
}

