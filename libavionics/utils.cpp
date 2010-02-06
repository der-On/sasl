#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


using namespace xa;


int xa::strToInt(const std::string &str, int dflt)
{
    int n;
    char *endptr;

    n = strtol(str.c_str(), &endptr, 10);
    if ((! str.c_str()[0]) || (endptr[0])) 
        //throw Exception("Invalid integer '" + str + "'");
        return dflt;
    else
        return n;
}


float xa::strToFloat(const std::string &str, float dflt)
{
    float n;
    char *endptr;

    n = strtof(str.c_str(), &endptr);
    if ((! str.c_str()[0]) || (endptr[0])) 
        return dflt;
    else
        return n;
}


double xa::strToDouble(const std::string &str, double dflt)
{
    double n;
    char *endptr;

    n = strtod(str.c_str(), &endptr);
    if ((! str.c_str()[0]) || (endptr[0])) 
        return dflt;
    else
        return n;
}


std::string xa::getDirectory(const std::string &fileName)
{
    int idx = fileName.find_last_of('/');
    if (0 > idx)
        idx = fileName.find_last_of('\\');
    std::string panelDir;
    if (0 < idx)
        return fileName.substr(0, idx);
    else
        return ".";
}


