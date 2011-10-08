#include "properties.h"

#include "avionics.h"
#include <string.h>


using namespace xa;


/// convert type name to type constant
/// returns -1 on errors
static int getPropType(const std::string &propType)
{
    if ("int" == propType)
        return PROP_INT;
    else if ("float" == propType)
        return PROP_FLOAT;
    else if ("double" == propType)
        return PROP_DOUBLE;
    else if ("string" == propType)
        return PROP_STRING;
    else
        return -1;
}


/// Lua wrapper for getProp
static int luaGetProp(lua_State *L)
{
    if ((! lua_isstring(L, 1)) || (! lua_isstring(L, 2))) {
        lua_pushnil(L);
        return 1;
    }

    std::string propName = lua_tostring(L, 1);
    int type = getPropType(lua_tostring(L, 2));

    if (-1 == type) {
        lua_pushnil(L);
        return 1;
    }

    SaslPropRef prop = getAvionics(L)->getProps().getProp(propName, type);

    if (prop)
        lua_pushlightuserdata(L, prop);
    else
        lua_pushnil(L);

    return 1;
}


/// Lua wrapper for createProp
static int luaCreateProp(lua_State *L)
{
    if (lua_isnil(L, 1) || lua_isnil(L, 2)) {
        lua_pushnil(L);
        return 1;
    }

    std::string propName = lua_tostring(L, 1);
    int type = getPropType(lua_tostring(L, 2));
    int maxLen = 0;
    int valArg = 3;
    if (PROP_STRING == type) {
        maxLen = lua_tonumber(L, 3);
        valArg++;
    }

    SaslPropRef prop = getAvionics(L)->getProps().createProp(propName, type, maxLen);

    if (prop) {
        if (! lua_isnil(L, valArg)) {
            switch (type) {
                case PROP_INT: {
                        int value = lua_tointeger(L, valArg);
                        getAvionics(L)->getProps().setProp(prop, value);
                        break;
                    }
                case PROP_FLOAT: {
                        float value = (float)lua_tonumber(L, valArg);
                        getAvionics(L)->getProps().setProp(prop, value);
                        break;
                    }
                case PROP_DOUBLE: {
                        double value = lua_tonumber(L, valArg);
                        getAvionics(L)->getProps().setProp(prop, value);
                        break;
                    }
                case PROP_STRING: {
                        const char *value = lua_tostring(L, valArg);
                        if (value) 
                            getAvionics(L)->getProps().setProp(prop, value);
                        break;
                    }
            }
        }
        lua_pushlightuserdata(L, prop);
    } else
        lua_pushnil(L);

    return 1;
}

/// Lua wrapper for createProp
static int luaCreateFuncProp(lua_State *L)
{
    if (lua_isnil(L, 1) || lua_isnil(L, 2) || (! lua_isfunction(L, 3)) ||
            (! lua_isfunction(L, 4))) {
        lua_pushnil(L);
        return 1;
    }

    Luna &lua = getAvionics(L)->getLuna();

    std::string propName = lua_tostring(L, 1);
    int type = getPropType(lua_tostring(L, 2));
    int maxSize = lua_tonumber(L, 5);
    lua_pushvalue(L, 3);
    int getter = lua.addRef();
    lua_pushvalue(L, 4);
    int setter = lua.addRef();
   
    SaslPropRef p = getAvionics(L)->getProps().registerFuncProp(propName, type, 
            maxSize, getter, setter);
    if (p)
        lua_pushlightuserdata(L, p);
    else
        lua_pushnil(L);

    return 1;
}


/// Lua wrapper for freeProp
static int luaFreeProp(lua_State *L)
{
    if (! lua_islightuserdata(L, 1))
        return 0;
    
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    if (prop)
        getAvionics(L)->getProps().freeProp(prop);

    return 0;
}


/// Lua wrapper for getPropi
static int luaGetPropi(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    int dflt = 0;
    
    if (! lua_isnil(L, 2))
        dflt = (int)lua_tonumber(L, 2);

    lua_pushnumber(L, getAvionics(L)->getProps().getPropi(prop, dflt));

    return 1;
}


/// Lua wrapper for setPropi
static int luaSetPropi(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    int value = (int)lua_tonumber(L, 2);
    getAvionics(L)->getProps().setProp(prop, value);
    return 0;
}


/// Lua wrapper for getPropf
static int luaGetPropf(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    float dflt = 0;
    
    if (! lua_isnil(L, 2))
        dflt = (float)lua_tonumber(L, 2);

    lua_pushnumber(L, getAvionics(L)->getProps().getPropf(prop, dflt));

    return 1;
}


/// Lua wrapper for setPropf
static int luaSetPropf(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    float value = (float)lua_tonumber(L, 2);
    getAvionics(L)->getProps().setProp(prop, value);
    return 0;
}

/// Lua wrapper for getPropd
static int luaGetPropd(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    float dflt = 0;
    
    if (! lua_isnil(L, 2))
        dflt = (double)lua_tonumber(L, 2);

    lua_pushnumber(L, getAvionics(L)->getProps().getPropd(prop, dflt));

    return 1;
}


/// Lua wrapper for setPropf
static int luaSetPropd(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    double value = (float)lua_tonumber(L, 2);
    getAvionics(L)->getProps().setProp(prop, value);
    return 0;
}

/// Lua wrapper for getProps
static int luaGetProps(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    const char *dflt = "";
    
    if (! lua_isnil(L, 2))
        dflt = lua_tostring(L, 2);

    lua_pushstring(L, getAvionics(L)->getProps().getProps(prop, dflt).c_str());

    return 1;
}


/// Lua wrapper for setProps
static int luaSetProps(lua_State *L)
{
    SaslPropRef prop = (SaslPropRef)lua_touserdata(L, 1);
    const char *value = lua_tostring(L, 2);
    getAvionics(L)->getProps().setProp(prop, value);
    return 0;
}



void xa::exportPropsToLua(Luna &lua)
{
    lua_State *L = lua.getLua();

    lua_register(L, "findProp", luaGetProp);
    lua_register(L, "createProp", luaCreateProp);
    lua_register(L, "createFuncProp", luaCreateFuncProp);
    lua_register(L, "freeProp", luaFreeProp);
    lua_register(L, "getPropi", luaGetPropi);
    lua_register(L, "setPropi", luaSetPropi);
    lua_register(L, "getPropf", luaGetPropf);
    lua_register(L, "setPropf", luaSetPropf);
    lua_register(L, "getPropd", luaGetPropd);
    lua_register(L, "setPropd", luaSetPropd);
    lua_register(L, "getProps", luaGetProps);
    lua_register(L, "setProps", luaSetProps);
}


Properties::Properties(Luna &lua): lua(lua)
{
    propsCallbacks = NULL;
    props = NULL;
}


Properties::~Properties()
{
    if (propsCallbacks && propsCallbacks->props_done)
        propsCallbacks->props_done(props);

    for (std::list<FuncPropHandler>::iterator i = funcProps.begin();
            i != funcProps.end(); i++) 
    {
        FuncPropHandler &h = *i;
        lua.unRef(h.getter);
        lua.unRef(h.setter);
    }
}


void Properties::setProps(struct SaslPropsCallbacks *callbacks, SaslProps p)
{
    if (propsCallbacks && propsCallbacks->props_done)
        propsCallbacks->props_done(props);

    propsCallbacks = callbacks;
    props = p;
}


SaslPropRef Properties::getProp(const std::string &name, int type)
{
    if (! (propsCallbacks && props))
        return NULL;

    return propsCallbacks->get_prop_ref(props, name.c_str(), type);
}


SaslPropRef Properties::createProp(const std::string &name, int type, int maxSize)
{
    if (! (propsCallbacks && props))
        return NULL;

    return propsCallbacks->create_prop(props, name.c_str(), type, maxSize);
}


void Properties::freeProp(SaslPropRef prop)
{
    if ((! prop) || (! propsCallbacks))
        return;

    propsCallbacks->free_prop_ref(prop);
}


int Properties::getPropi(SaslPropRef prop, int dflt, int *err)
{
    int localErr;
    if (! err)
        err = &localErr;
    *err = 0;

    if (! prop)
        return dflt;

    if (! (propsCallbacks && props)) {
        if (err) 
            *err = -1;
        return dflt;
    }

    int res = propsCallbacks->get_prop_int(prop, err);
    if (*err)
        res = dflt;
    return res;
}


int Properties::setProp(SaslPropRef prop, int value)
{
    if (! prop)
        return 0;
    
    if (! (propsCallbacks && props))
        return 0;

    return propsCallbacks->set_prop_int(prop, value);
}


float Properties::getPropf(SaslPropRef prop, float dflt, int *err)
{
    int localErr;
    if (! err)
        err = &localErr;
    *err = 0;

    if (! prop) {
        *err = -1;
        return dflt;
    }

    if (! (propsCallbacks && props)) {
        *err = -1;
        return dflt;
    }

    float res = propsCallbacks->get_prop_float(prop, err);
    if (*err)
        res = dflt;
    return res;
}


int Properties::setProp(SaslPropRef prop, float value)
{
    if (! prop)
        return 0;
    
    if (! (propsCallbacks && props))
            return 0;

    return propsCallbacks->set_prop_float(prop, value);
}

float Properties::getPropd(SaslPropRef prop, double dflt, int *err)
{
    int localErr;
    if (! err)
        err = &localErr;
    *err = 0;

    if (! prop) {
        *err = -1;
        return dflt;
    }

    if (! (propsCallbacks && props)) {
        *err = -1;
        return dflt;
    }

    double res = propsCallbacks->get_prop_double(prop, err);
    if (*err)
        res = dflt;
    return res;
}


int Properties::setProp(SaslPropRef prop, double value)
{
    if (! prop)
        return 0;
    
    if (! (propsCallbacks && props))
        return 0;

    return propsCallbacks->set_prop_double(prop, value);
}


std::string Properties::getProps(SaslPropRef prop, const std::string &dflt, 
        int *err)
{
    int localErr;
    if (! err)
        err = &localErr;
    *err = 0;

    if (! prop)
        return dflt;

    if (! (propsCallbacks && props)) {
        *err = -1;
        return dflt;
    }

    int sz = propsCallbacks->get_prop_string(prop, NULL, 0, err);
#ifdef WINDOWS
    char *buf = (char*)alloca(sz + 1);
#else
    char buf[sz + 1];
#endif
    propsCallbacks->get_prop_string(prop, buf, sz, err);
    if (*err)
        return dflt;
    return buf;
}


int Properties::setProp(SaslPropRef prop, const std::string &value)
{
    if ((! prop) || (! (propsCallbacks && props)))
        return 0;
    
    return propsCallbacks->set_prop_string(prop, value.c_str());
}


int Properties::update()
{
    if (! (propsCallbacks && props))
        return 0;

    int err = 0;
    if (propsCallbacks->update_props)
        err = propsCallbacks->update_props(props);

    return err;
}


static int propGetterCallback(int type, void *buf, int maxSize, void *ref)
{
    Properties::FuncPropHandler *handler = (Properties::FuncPropHandler*)ref;
    if (! handler)
        return 0;

    Luna &lua = handler->properties->getLua();
    lua_State *L = lua.getLua();
    
    lua.getRef(handler->getter);
    
    if (lua_pcall(L, 0, 1, 0))
        getAvionics(L)->getLog().error(
                "Error calling property getter: %s\n", lua_tostring(L, -1));
    else {
        switch (type) {
            case PROP_INT: {
                    int v = lua_tointeger(L, -1);
                    if (buf && (maxSize >= (int)sizeof(v)))
                        memcpy(buf, &v, sizeof(v));
                    return sizeof(v);
                }
            case PROP_FLOAT: {
                    float v = (float)lua_tonumber(L, -1);
                    if (buf && (maxSize >= (int)sizeof(v)))
                        memcpy(buf, &v, sizeof(v));
                    return sizeof(v);
                }
            case PROP_DOUBLE: {
                    double v = lua_tonumber(L, -1);
                    if (buf && (maxSize >= (int)sizeof(v)))
                        memcpy(buf, &v, sizeof(v));
                    return sizeof(v);
                }
            case PROP_STRING: {
                    const char *v = lua_tostring(L, -1);
                    int len = strlen(v);
                    if (buf && (maxSize >= (int)len))
                        memcpy(buf, v, len + 1);
                    return len + 1;
                }
        }
        lua_pop(L, 1);
    }

    return 0;
}


static void propSetterCallback(int type, void *buf, int size, void *ref)
{
    Properties::FuncPropHandler *handler = (Properties::FuncPropHandler*)ref;
    if ((! handler) || (! buf))
        return;

    Luna &lua = handler->properties->getLua();
    lua_State *L = lua.getLua();
    lua.getRef(handler->setter);

    switch (type) {
        case PROP_INT: 
            lua_pushinteger(L, *((int*)buf));
            break;
        case PROP_FLOAT:
            lua_pushnumber(L, *((float*)buf));
            break;
        case PROP_DOUBLE:
            lua_pushnumber(L, *((double*)buf));
            break;
        case PROP_STRING:
            lua_pushstring(L, ((char*)buf));
            break;
    }
    
    if (lua_pcall(lua.getLua(), 1, 0, 0))
        getAvionics(L)->getLog().error(
                "Error calling property setter: %s\n", lua_tostring(L, -1));
}

SaslPropRef Properties::registerFuncProp(const std::string &name, int type, 
        int maxSize, int getter, int setter)
{
    if (! (propsCallbacks && props))
        return 0;

    FuncPropHandler handler;
    handler.properties = this;
    handler.getter = getter;
    handler.setter = setter;

    funcProps.push_back(handler);

    return propsCallbacks->create_func_prop(props, name.c_str(),
            type, maxSize, propGetterCallback, propSetterCallback, 
            &(funcProps.back()));
}


void Properties::destroyFuncProp(FuncPropHandler *handler)
{
    for (std::list<FuncPropHandler>::iterator i = funcProps.begin(); 
            i != funcProps.end(); i++)
    {
        FuncPropHandler *h = &(*i);
        if ((h == handler)) {
            lua.unRef(h->getter);
            lua.unRef(h->setter);
            funcProps.erase(i);
            break;
        }
    }
}

