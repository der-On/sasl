#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <typeinfo>
#include "exception.h"


namespace xa {


/// Convert value to string
template<typename T>
inline std::string toString(const T& x)
{
    std::ostringstream o;
    if (!(o << x))
        throw Exception(std::string("toString(") + typeid(x).name() + ")");
    return o.str();
}


/// Like to String, but more usefull error report
template<typename T>
inline std::string toString(const T& x, const char *file, int line)
{
    std::ostringstream o;
    if (!(o << x))
        throw Exception(std::string(file) + ":" + toString(line) + ": " +
                std::string("toString(") + typeid(x).name() + ")");
    return o.str();
}

/// Converts value to string and throws exception on errors
#define STRING(x) toString(x, __FILE__, __LINE__)

/// convert number to integer
int strToInt(const std::string &str, int dflt=0);


/// Extract directory from full path to file
std::string getDirectory(const std::string &fileName);

};


#endif

