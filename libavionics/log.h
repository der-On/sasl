#ifndef __LOG_H__
#define __LOG_H__

#include "luna.h"
#include "libavcallbacks.h"

namespace xa {

// logger
class Log
{
    private:
        /// logger function
        sasl_log_callback callback;

        /// Data for logger function
        void *ref;

    public:
        /// Create default logger
        Log();

    public:
        /// log message
        void log(int level, const char *message, va_list args);

        /// log debug message
        void debug(const char *message, ...);
        
        /// log info message
        void info(const char *message, ...);
        
        /// log warning message
        void warning(const char *message, ...);
        
        /// log error message
        void error(const char *message, ...);

    public:
        /// set logger function
        void setLogger(sasl_log_callback callback, void *ref);
        
        /// get logger function
        sasl_log_callback getLogger(void **ref);

        /// register logger functions in Lua
        void exportToLua(Luna &lua);
};

};

#endif

