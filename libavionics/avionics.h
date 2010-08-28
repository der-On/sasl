#ifndef __AVIONICS_H__
#define __AVIONICS_H__


#include <string>
#include "luna.h"
#include "rttimer.h"
#include "clickemul.h"
#include "texture.h"
#include "font.h"
#include "xcallbacks.h"
#include "properties.h"
#include "propsserv.h"
#include "commands.h"


namespace xa {


/// X-Avionics main class
class Avionics
{
    private:
        /// path to avionics data directory
        std::string path;

        /// Width of panel in pixels
        int panelWidth;

        /// Height of panel in pixels
        int panelHeight;

        /// Width of popup layer in pixels
        int popupWidth;

        /// Height of popup layer in pixels
        int popupHeight;

        /// Lua state
        Luna lua;

        /// True to enable click emulation
        bool clickEmulation;

        /// Time counter
        RtTimer timer;

        /// Muse click events emulator
        ClickEmulator clickEmulator;

        /// Textures cache
        TextureManager textureManager;
        
        /// Fonts loader and cache
        FontManager fontManager;

        /// Background color
        float bgR, bgG, bgB, bgA;

        /// Properties subsystem
        Properties properties;

        /// Properties server
        PropsServer server;

        /// Commands API
        Commands commands;

        /// Graphics functions
        XaGraphicsCallbacks *graphics;

        /// Time passed since last garbage collection
        long lastGcTime;

    public:
        /// Initialize avionics internal data
        Avionics(const std::string &path);

        /// Destroy avionics
        ~Avionics();

    public:
        /// Setup panel resolution.
        /// \param width - width of panel, default is 1024
        /// \param height - height of panel, default is 768
        void setPanelResolution(int width, int height);
        
        /// Setup popup resolution.
        /// \param width - width of popup layer, default is 1024
        /// \param height - height of popup layer, default is 768
        void setPopupResolution(int width, int height);

        /// Load aircraft panel from specified location
        void loadPanel(const std::string &path);

        /// Draw panel
        void draw(int stage);

        /// Called on mouse button up event
        bool onMouseUp(int x, int y, int button, int layer);

        /// Called on mouse button down event
        bool onMouseDown(int x, int y, int button, int layer);
        
        /// Called on mouse move event
        bool onMouseMove(int x, int y, int layer);

        /// Called on mouse button click event
        bool onMouseClick(int x, int y, int button, int layer);

        /// Enable or disable mouse click emulator
        void enableClickEmulator(bool enable);

        /// Set click emulation parameters
        /// \param delay number of milliseconds before second click event
        /// \param period number of milliseconds before third and other click events
        void setClickParams(int delay, int period);

        /// Called on keyboard press event.
        /// \param charCode code of pressed character
        /// \param keyCode scan code of pressed button
        /// Returns true of event was handled
        bool onKeyDown(int charCode, int keyCode);

        /// Called on keyboard up event.
        /// \param charCode code of pressed character
        /// \param keyCode scan code of pressed button
        /// Returns true of event was handled
        bool onKeyUp(int charCode, int keyCode);

        /// Returns texture manager
        TextureManager* getTextureManager() { return &textureManager; }

        /// Returns font manager
        FontManager* getFontManager() { return &fontManager; }

        /// Returns Lua state
        lua_State* getLua() { return lua.getLua(); }

        /// Returns properties
        Properties& getProps() { return properties; }

        /// Set background color for textures
        void setBackgroundColor(float r, float g, float b, float a);
        
        /// Returns background color for textures
        void getBackgroundColor(float &r, float &g, float &b, float &a);

        /// Enable or disable clickable regions highlight
        void setShowClickable(bool show);

        /// Proceed events on each frame
        void update();

        /// Start props server
        int startPropsServer(int port, const std::string &secret);

        /// Stop ptops server
        void stopPropsServer();

        /// Returns commands API
        Commands& getCommands() { return commands; };

        /// Set commands callbacks
        void setCommandsCallbacks(XaCommandCallbacks *callbacks,
                void *data);

        /// Returns lua wrapper
        Luna& getLuna() { return lua; };

        /// Set graphics callbacks
        void setGraphicsCallbacks(XaGraphicsCallbacks *callbacks);

        /// Returns grpahics callbacks.
        struct XaGraphicsCallbacks* getGraphics() { return graphics; };

    private:
        /// Add path to components search list
        void addSearchPath(const std::string &path);
        
        /// Add path to images search list
        void addSearchImagePath(const std::string &path);
};

};

#endif

