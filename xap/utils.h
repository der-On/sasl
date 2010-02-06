#ifndef __UTILS_H__
#define __UTILS_H__


#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <typeinfo>


namespace xap {


/// Convert value to string
template<typename T>
inline std::string toString(const T& x)
{
    std::ostringstream o;
    if (!(o << x))
        return "";
    return o.str();
}

/// convert number to integer
int strToInt(const std::string &str, int dflt=0);

/// convert number to float
float strToFloat(const std::string &str, float dflt=0);

/// convert number to double
double strToDouble(const std::string &str, double dflt=0);


/// convert integer to string
std::string intToStr(int value);


};

#endif

