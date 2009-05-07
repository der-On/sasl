#include "gui.h"

#include "xpsdk.h"
#include "main.h"
#include "options.h"
#include "utils.h"
#include "xavionics.h"


using namespace xap;


#define START_MENU (void*)1
#define STOP_MENU (void*)2
#define OPTIONS_MENU (void*)3


/// Plugin menu
static XPLMMenuID xapMenu;

// menus items IDs
static int startMenuItem;
static int stopMenuItem;
static int optionsMenuItem;


// Dialog window handler
static XPWidgetID optionsWindow = NULL;

// login text field handler
static XPWidgetID portField;

// Password text field handler
static XPWidgetID secretField;

// Password text field handler
static XPWidgetID autoStartCheckBox;

// Button which logs in
static XPWidgetID okButton;


// called on login dialog events
static int optionsWindowHandler(XPWidgetMessage message, XPWidgetID widget,
        long param1, long param2)
{
    switch (message) {
        case xpMessage_CloseButtonPushed:
            XPHideWidget(optionsWindow);
            return 1;

        case xpMsg_PushButtonPressed:
            XPHideWidget(optionsWindow);
            if ((long)okButton == param1) {
                char buf[100];

                XPGetWidgetDescriptor(secretField, buf, 100);
                std::string secret = buf;

                XPGetWidgetDescriptor(portField, buf, 100);
                int port = strToInt(buf);

                int btnState = XPGetWidgetProperty(autoStartCheckBox, 
                        xpProperty_ButtonState, NULL);
                bool autoStart = btnState;

                options.setSecret(secret);
                options.setPort(port);
                options.enableAutoStartServer(autoStart);
                options.save();
            }
            return 1;
    }

    return 0;
}


/// Create server option dialog
static void createOptionsDialog()
{
    int x = 100;
    int y = 700;
    int w = 300;
    int h = 195;

    int x2 = x + w;
    int y2 = y - h;

    optionsWindow = XPCreateWidget(x, y, x2, y2,
            1, "X-Avionics Network Options", 1, NULL, 
            xpWidgetClass_MainWindow);
    XPSetWidgetProperty(optionsWindow, xpProperty_MainWindowHasCloseBoxes, 1);

    XPCreateWidget(x + 10, y - 30, x2 - 10, y - 145,
            1, "", 0, optionsWindow, xpWidgetClass_SubWindow);
    
    XPCreateWidget(x + 20, y - 50, x + 100, y - 65,
            1, "Port:", 0, optionsWindow, xpWidgetClass_Caption);
    portField = XPCreateWidget(x + 100, y - 50, x2 - 20, y - 65,
            1, intToStr(options.getPort()).c_str(), 0, optionsWindow, 
            xpWidgetClass_TextField);
    XPSetWidgetProperty(portField, xpProperty_MaxCharacters, 30);
    
    XPCreateWidget(x + 20, y - 80, x + 100, y - 95,
            1, "Secret:", 0, optionsWindow, xpWidgetClass_Caption);
    secretField = XPCreateWidget(x + 100, y - 80, x2 - 20, y - 95,
            1, options.getSecret().c_str(), 0, optionsWindow, 
            xpWidgetClass_TextField);
    XPSetWidgetProperty(secretField, xpProperty_PasswordMode, 1);
    XPSetWidgetProperty(secretField, xpProperty_MaxCharacters, 30);
    
    XPCreateWidget(x + 20, y - 110, x + 100, y - 125,
            1, "Auto start:", 0, optionsWindow, xpWidgetClass_Caption);
    autoStartCheckBox = XPCreateWidget(x + 100, y - 110, x + 115, y - 125,
            1, "", 0, optionsWindow, xpWidgetClass_Button);
    XPSetWidgetProperty(autoStartCheckBox, xpProperty_ButtonType, xpRadioButton);
    XPSetWidgetProperty(autoStartCheckBox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
    XPSetWidgetProperty(autoStartCheckBox, xpProperty_ButtonState, 
            options.isAutoStartServer());

    okButton = XPCreateWidget(x + 70, y - 160, x + 140, y - 180,
            1, "OK", 0, optionsWindow, xpWidgetClass_Button);
    XPCreateWidget(x + 160, y - 160, x + 230, y - 180,
            1, "Cancel", 0, optionsWindow, xpWidgetClass_Button);

    XPAddWidgetCallback(optionsWindow, optionsWindowHandler);
}


static void showOptionsDialog()
{
    if (! optionsWindow)
        createOptionsDialog();
    XPShowWidget(optionsWindow);
}


static void destroyOptionsDialog()
{
    if (optionsWindow) {
        XPDestroyWidget(optionsWindow, 1);
        optionsWindow = NULL;
    }
}


// Process menu events
static void xapMenuCallback(void *menuRef, void *param)
{
    if (OPTIONS_MENU == param)
        showOptionsDialog();
    else if (START_MENU == param) {
        if (xa_start_netprop_server(xa, options.getPort(), 
                    options.getSecret().c_str())) 
        {
            printf("error starting server\n");
        } else
            enableMenus(true);
    } else if (STOP_MENU == param) {
        xa_stop_netprop_server(xa);
        enableMenus(false);
    }
}


void xap::initGui()
{
    XPLMMenuID pluginsMenu = XPLMFindPluginsMenu();
    int subMenuItem = XPLMAppendMenuItem(pluginsMenu, "X-Avionics", 
            NULL, 1);
    xapMenu = XPLMCreateMenu("X-Avionics", pluginsMenu, subMenuItem,
            xapMenuCallback, NULL);
    startMenuItem = XPLMAppendMenuItem(xapMenu, "Start server", START_MENU, 1);
    stopMenuItem = XPLMAppendMenuItem(xapMenu, "Stop server", STOP_MENU, 1);
    optionsMenuItem = XPLMAppendMenuItem(xapMenu, "Server options...", 
            OPTIONS_MENU, 1);

    enableMenus(options.isAutoStartServer());
}


void xap::enableMenus(bool serverRunning)
{
    XPLMEnableMenuItem(xapMenu, startMenuItem, ! serverRunning);
    XPLMEnableMenuItem(xapMenu, stopMenuItem, serverRunning);
}

void xap::doneGui()
{
    XPLMDestroyMenu(xapMenu);
    destroyOptionsDialog();
}

