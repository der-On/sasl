#include "cmdline.h"

#include "utils.h"
#include <string.h>
#include <stdlib.h>


using namespace slava;


slava::CmdLine::CmdLine(int argc, char *argv[]): 
    netHost(""), netPort(45829), secret(""), 
    screenWidth(800), screenHeight(600),
    fullscreen(false), panel("panel.lua"), dataDir("./data")
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
        else {
            printf("Invalid option '%s'.\n", argv[i]);
            exit(1);
        }
    }
}

