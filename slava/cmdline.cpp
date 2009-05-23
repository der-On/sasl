#include "cmdline.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "../version.h"


using namespace slava;


/// Print SLAVA version and exit
static void printVersion()
{
#ifdef SNAPSHOT
#define xstr(s) str(s)
#define str(s) #s
    printf("Stand-Alone Avionics Application snapshot %i.%i.%i %s\n", 
            VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, xstr(SNAPSHOT));
#undef str
#undef xstr
#else
    printf("Stand-Alone Avionics Application v%i.%i.%i\n", 
            VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
#endif
    exit(0);
}


/// Print short command line help and exit
static void printHelp()
{
    printf("USAGE:\n");
    printf("  slava [options]\n");
    printf("OPTIONS:\n");
    printf("  --host <hostname>    - address of flight simulator server\n");
    printf("  --port <portnumber>  - port number at flight simulator server\n");
    printf("  --secret <password>  - flight simulator password\n");
    printf("  --width <pixels>     - width of window\n");
    printf("  --height <pixels>    - height of window\n");
    printf("  --fullscreen         - enable fullscreen mode\n");
    printf("  --panel <file>       - path to panel lua file\n");
    printf("  --data <path>        - location of sasl data dir\n");
    printf("  --fps <limit>        - limit maximum FPS (use 0 for unlimited)\n");
    printf("  --version            - print version number\n");
    printf("  --help               - print this help\n");
    exit(0);
}


slava::CmdLine::CmdLine(int argc, char *argv[]): 
    netHost(""), netPort(45829), secret(""), 
    screenWidth(800), screenHeight(600),
    fullscreen(false), panel("panel.lua"), dataDir("./data"),
    targetFps(60)
{
    for (int i = 1; i < argc; i++) {
        if (! argv[i])
            continue;

        if ((! strcmp(argv[i], "--host")) && (i < argc - 1))
            netHost = std::string(argv[++i]);
        else if ((! strcmp(argv[i], "--port")) && (i < argc - 1))
            netPort = strToInt(argv[++i]);
        else if ((! strcmp(argv[i], "--secret")) && (i < argc - 1))
            secret = std::string(argv[++i]);
        else if ((! strcmp(argv[i], "--width")) && (i < argc - 1))
            screenWidth = strToInt(argv[++i]);
        else if ((! strcmp(argv[i], "--height")) && (i < argc - 1))
            screenHeight = strToInt(argv[++i]);
        else if (! strcmp(argv[i], "--fullscreen"))
            fullscreen = true;
        else if ((! strcmp(argv[i], "--panel")) && (i < argc - 1))
            panel = std::string(argv[++i]);
        else if ((! strcmp(argv[i], "--data")) && (i < argc - 1))
            dataDir = std::string(argv[++i]);
        else if ((! strcmp(argv[i], "--fps")) && (i < argc - 1))
            targetFps = strToInt(argv[++i]);
        else if (! strcmp(argv[i], "--version"))
            printVersion();
        else if (! strcmp(argv[i], "--help"))
            printHelp();
        else {
            printf("Invalid option '%s'.\n", argv[i]);
            exit(1);
        }
    }
}

