#ifndef __CMD_LINE_H__
#define __CMD_LINE_H__


#include <string>

namespace slava {


class CmdLine
{
    private:
        /// Host running simulator
        std::string netHost;

        /// remote simulator port
        int netPort;

        /// remove simulator password
        std::string secret;

        /// Width of screen to use
        int screenWidth;

        /// Height of screen to use
        int screenHeight;

        /// is fullscreen mode requested
        bool fullscreen;

        /// panel file name
        std::string panel;

        /// path to data dir
        std::string dataDir;

        /// Desired FPS
        int targetFps;

    public:
        /// Parse command line
        CmdLine(int argc, char *argv[]);

    public:
        /// Returns remote simulator host name
        const std::string& getNetHost() const { return netHost; }
        
        /// Returns remote simulator port
        int getNetPort() const { return netPort; }
        
        /// Returns remote simulator password
        const std::string& getNetSecret() const { return secret; }
        
        /// Returns width of screen
        int getScreenWidth() const { return screenWidth; }
        
        /// Returns height of screen
        int getScreenHeight() const { return screenHeight; }
        
        /// Returns true if fullscreen mode requested
        bool isFullscreen() const { return fullscreen; }
        
        /// Returns panel file name
        const std::string& getPanel() const { return panel; }
        
        /// Returns path to data dir
        const std::string& getDataDir() const { return dataDir; }
        
        /// Returns desired FPS
        int getTargetFps() const { return targetFps; }
};

};

#endif

