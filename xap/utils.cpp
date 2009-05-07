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


std::string xap::intToStr(int value)
{
    char buf[50];
    sprintf(buf, "%i", value);
    return buf;
}


