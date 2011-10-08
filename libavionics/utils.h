#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <sstream>
#include <string>


namespace xa {


/// Convert value to string
template<typename T>
inline std::string toString(const T& x)
{
    std::ostringstream o;
    o << x;
    return o.str();
}


/// Like to String, but more usefull error report
template<typename T>
inline std::string toString(const T& x, const char *file, int line)
{
    std::ostringstream o;
    o << x;
    return o.str();
}

/// Converts value to string and throws exception on errors
#define STRING(x) toString(x, __FILE__, __LINE__)

/// convert number to integer
int strToInt(const std::string &str, int dflt=0);

/// convert number to float
float strToFloat(const std::string &str, float dflt=0);

/// convert number to double
double strToDouble(const std::string &str, double dflt=0);

/// Extract directory from full path to file
std::string getDirectory(const std::string &fileName);

};


#endif

