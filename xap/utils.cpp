#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


using namespace xap;


int xap::strToInt(const std::string &str, int dflt)
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

float xap::strToFloat(const std::string &str, float dflt)
{
    float n;
    char *endptr;

#ifdef WINDOWS
    n = (float)strtod(str.c_str(), &endptr);
#else
    n = strtof(str.c_str(), &endptr);
#endif
    if ((! str.c_str()[0]) || (endptr[0])) 
        return dflt;
    else
        return n;
}


double xap::strToDouble(const std::string &str, double dflt)
{
    double n;
    char *endptr;

    n = strtod(str.c_str(), &endptr);
    if ((! str.c_str()[0]) || (endptr[0])) 
        return dflt;
    else
        return n;
}


std::string xap::intToStr(int value)
{
    char buf[50];
    sprintf(buf, "%i", value);
    return buf;
}


