#ifndef __UTILS_H__
#define __UTILS_H__


#include <string>


namespace xap {

/// convert number to integer
int strToInt(const std::string &str, int dflt=0);


/// convert integer to string
std::string intToStr(int value);


};

#endif

