#include "log.h"

#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include "avionics.h"
#include "libavconsts.h"


using namespace xa;


// default logger
static void defaultLogger(int level, const char *message)
{
    if (! message)
        return;

    switch (level) {
        case LOG_DEBUG: printf("DEBUG: "); break;
        case LOG_INFO: printf("INFO: "); break;
        case LOG_WARNING: printf("WARNING: "); break;
        default: printf("ERROR: ");
    }

    printf("%s\n", message);
}

Log::Log()
{
    setLogger(NULL, NULL);
}


void Log::log(int level, const char *message, va_list args)
{
	va_list cp;
	va_copy(cp, args);
    int msgLen = vsnprintf(NULL, 0, message, cp);
    va_end(cp);
    char *buf = (char*)alloca(msgLen + 1);
    vsnprintf(buf, msgLen + 1, message, args);
    callback(level, buf);
}


void Log::debug(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    log(LOG_DEBUG, message, args);
    va_end(args);
}


void Log::info(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    log(LOG_INFO, message, args);
    va_end(args);
}


void Log::warning(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    log(LOG_WARNING, message, args);
    va_end(args);
}


void Log::error(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    log(LOG_ERROR, message, args);
    va_end(args);
}


void Log::setLogger(sasl_log_callback callback, void *ref)
{
    this->callback = callback;
    this->ref = ref;
    if (! callback) {
        this->callback = defaultLogger;
        this->ref = NULL;
    }
}


sasl_log_callback Log::getLogger(void **refPtr)
{
    if (refPtr)
        *refPtr = ref;
    return callback;
}


static std::string getArgs(lua_State *L)
{
    std::string str;
    
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++) {
        const char *s;

        if (i > 1)
            str += "\t";
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        if (! s)
            str += "(error)";
        str += s;
        lua_pop(L, 1);  /* pop result */
    }

    return str;
}


/// print debug message
static int luaDebug(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    avionics->getLog().debug(getArgs(L).c_str());
    return 0;
}


/// print info message
static int luaInfo(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    avionics->getLog().info(getArgs(L).c_str());
    return 0;
}

/// print warning message
static int luaWarning(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    avionics->getLog().warning(getArgs(L).c_str());
    return 0;
}

/// print error message
static int luaError(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    avionics->getLog().error(getArgs(L).c_str());
    return 0;
}


void Log::exportToLua(Luna &lua)
{
    lua_State *L = lua.getLua();
    lua_register(L, "logDebug", luaDebug);
    lua_register(L, "logInfo", luaInfo);
    lua_register(L, "logWarning", luaWarning);
    lua_register(L, "logError", luaError);
    lua_register(L, "print", luaInfo);
}

