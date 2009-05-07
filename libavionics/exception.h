#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__


#include <string>
#include <stdexcept>


namespace xa {

/// Just exception
class Exception: public std::exception
{
    private:
        /// Error message
        std::string reason;

    public:
        Exception(const std::string &message): reason(message) { };
        
        virtual ~Exception() throw () { };

    public:
        /// Returns error message as C string
        virtual const char *what() const throw () { return reason.c_str(); }
};

};

#include "utils.h"

/// Throws exception
#define EXCEPTION(msg) { throw Exception(std::string(__FILE__) + ":" + toString(__LINE__) + ": " + std::string(msg)); }


#endif

