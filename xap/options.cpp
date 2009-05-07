#include "options.h"
#include <fstream>


using namespace xap;


Options::Options(const std::string &path): path(path), port(45829), secret(""),
    autoStartServer(false)
{
}


void Options::load()
{
    char buf[255];

    std::ifstream f(path.c_str(), std::ios::in);
    if (! f.good())
        return;

    f.getline(buf, 255);
    secret = buf;

    f >> port;
    f >> autoStartServer;
    
    f.close();
}


void Options::save()
{
    std::ofstream f(path.c_str(), std::ios::out);
    if (! f.good())
        return;

    f << secret << std::endl;
    f << port << std::endl;
    f << autoStartServer << std::endl;

    f.close();
}

