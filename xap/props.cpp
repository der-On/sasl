#include <list>
#include <map>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "xpsdk.h"

#include "props.h"
#include "libavionics.h"
#include "utils.h"



using namespace xap;


struct XPlaneProps;


/// Reference to X-Plane property 
struct Property {
    /// X-Plane property reference
    XPLMDataRef ref;

    /// Index of property in array or 0 if property is not array
    int index;

    /// Link to properties structure
    XPlaneProps *parent;
};


/// Value of property
struct Value {
    /// Property as int
    int intValue;

    /// Property as float
    float floatValue;

    /// Property as double
    double doubleValue;

    /// property as string
    std::string stringValue;
};


/// Self-created property
struct CustomProperty
{
    /// Reference to property for unregistering
    XPLMDataRef ref;

    /// property value
    Value data;
};


/// Self-created functional property
struct FuncProperty
{
    /// Reference to property for unregistering
    XPLMDataRef ref;

    /// property getter
    sasl_prop_getter_callback getter;

    /// property setter
    sasl_prop_setter_callback setter;

    /// reference for callbacks
    void *data;
};


/// List of referneces to properties
typedef std::list<Property*> PropsList;


/// List of self-created properties
typedef std::map<std::string, CustomProperty*> CustomPropsMap;

/// List of functional properties
typedef std::list<FuncProperty*> FuncPropsList;



/// delayed set property value command
struct SetPropCmd
{
    /// property reference
    Property *property;

    /// type of value
    int type;

    /// value to set
    Value data;
    
    SetPropCmd(Property *prop, int value) {
        property = prop;
        type = PROP_INT;
        data.intValue = value;
    };

    SetPropCmd(Property *prop, float value) {
        property = prop;
        type = PROP_FLOAT;
        data.floatValue = value;
    };
    
    SetPropCmd(Property *prop, double value) {
        property = prop;
        type = PROP_DOUBLE;
        data.doubleValue = value;
    };
    
    SetPropCmd(Property *prop, const std::string &value) {
        property = prop;
        type = PROP_STRING;
        data.stringValue = value;
    };
};

/// List of set prop commands
typedef std::list<struct SetPropCmd> PropsToSet;


/// X-Plane properties info
struct XPlaneProps {
    /// References to properties
    PropsList props;

    /// user created properties
    CustomPropsMap customProps;

    /// user created callback properties
    FuncPropsList funcProps;

    /// true if properties system was initialized
    bool initialized;

    /// list of properties to set
    PropsToSet propsToSet;
};


/// Initialize properties structure
SaslProps xap::propsInit()
{
    XPlaneProps *props = new XPlaneProps;
    props->initialized = false;
    return props;
}


/// Free properties structure
void xap::propsDone(SaslProps props)
{
    XPlaneProps *p = (XPlaneProps*)props;
    if (! p)
        return;

    for (PropsList::iterator i = p->props.begin(); i != p->props.end(); ++i)
        delete *i;
    
    for (FuncPropsList::iterator i = p->funcProps.begin(); 
            i != p->funcProps.end(); ++i)
    {
        XPLMUnregisterDataAccessor((*i)->ref);
        delete *i;
    }
    
    for (CustomPropsMap::iterator i = p->customProps.begin(); 
            i != p->customProps.end(); ++i)
    {
        XPLMUnregisterDataAccessor((*i).second->ref);
        delete (*i).second;
    }

    if (p)
        delete p;
}

void xap::funcPropsDone(SaslProps props)
{
    XPlaneProps *p = (XPlaneProps*)props;
    if (! p)
        return;

    for (FuncPropsList::iterator i = p->funcProps.begin(); 
            i != p->funcProps.end(); ++i)
    {
        XPLMUnregisterDataAccessor((*i)->ref);
        delete *i;
    }
    p->funcProps.clear();
}


/// cut array index from string (if exists)
/// returns stripped index in index variable.
/// e.g. if name is "array[5]", name will be "array" and index will be 5
static void cutArrayIndex(std::string &name, int &index)
{
    unsigned int firstIdx = name.find_first_of('[');
    if (std::string::npos == firstIdx)
        return;
    
    unsigned int lastIdx = name.find_first_of('[', firstIdx);
    if (std::string::npos == lastIdx)
        return; // invalid index

    std::string idxStr = name.substr(firstIdx + 1, lastIdx - firstIdx - 1);
    index = atoi(idxStr.c_str());
    name.erase(firstIdx);
}


/// Finds reference to property
static SaslPropRef getPropRef(SaslProps props, const char *name, int type)
{
    XPlaneProps *p = (XPlaneProps*)props;
    if (! (p && name))
        return NULL;

    int index = 0;
    XPLMDataRef ref = XPLMFindDataRef(name);

    if (! ref) {
        std::string realName = name;
        cutArrayIndex(realName, index);

        ref = XPLMFindDataRef(realName.c_str());
        if (! ref)
            return NULL;
    }

    Property *prop = new Property;
    prop->ref = ref;
    prop->index = index;
    prop->parent = p;
    p->props.push_back(prop);
    return prop;
}


/// Destroy unneeded reference to pointer
static void freePropRef(SaslPropRef property)
{
    Property *prop = (Property*)property;
    if (! prop)
        return;

    XPlaneProps *p = prop->parent;
    if (! p)
        return;

    for (PropsList::iterator i = p->props.begin(); i != p->props.end(); ++i)
        if ((*i) == (Property*)prop) {
            p->props.erase(i);
            delete prop;
            return;
        }
}


/// Returne value of property as integer
static int getPropInt(SaslPropRef property, int *err)
{
    if (err)
        *err = 0;

    Property *prop = (Property*)property;
    if (! prop) {
        if (err)
            *err = 1;
        return 0;
    }

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);

    if (xplmType_Int & type)
        return XPLMGetDatai(prop->ref);
    
    if (xplmType_Float & type)
        return (int)XPLMGetDataf(prop->ref);
    
    if (xplmType_Double & type)
        return (int)XPLMGetDatad(prop->ref);

    if (xplmType_IntArray & type) {
        int val = 0;
        XPLMGetDatavi(prop->ref, &val, prop->index, 1);
        return val;
    }
    
    if (xplmType_FloatArray & type) {
        float val = 0;
        XPLMGetDatavf(prop->ref, &val, prop->index, 1);
        return (int)val;
    }

    if (xplmType_Data & type) {
        int len = XPLMGetDatab(prop->ref, NULL, 0, 0);
        if (0 < len) {
#ifdef WINDOWS
            char *buf = (char*)alloca(len + 1);
#else
            char buf[len + 1];
#endif
            XPLMGetDatab(prop->ref, buf, 0, len);
            buf[len] = 0;
            return strToInt(buf);
        } else
            return 0;
    }

    if (err)
        *err = 1;

    return 0;
}


/// Sets value of property as integer
/// Returns zero on cuccess or non-zero on error
static int setPropInt(SaslPropRef property, int value)
{
    Property *prop = (Property*)property;
    if (! prop)
        return -1;
    
    XPlaneProps *props = prop->parent;
    if (! props->initialized) 
        props->propsToSet.push_back(SetPropCmd(prop, value));

    if (! XPLMCanWriteDataRef(prop->ref))
        return -1;

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);

    if (xplmType_Int & type) {
        XPLMSetDatai(prop->ref, value);
        return 0;
    }
    
    if (xplmType_Float & type) {
        XPLMSetDataf(prop->ref, (float)value);
        return 0;
    }
    
    if (xplmType_Double & type) {
        XPLMSetDatad(prop->ref, (double)value);
        return 0;
    }

    if (xplmType_IntArray & type) {
        XPLMSetDatavi(prop->ref, &value, prop->index, 1);
        return 0;
    }
    
    if (xplmType_FloatArray & type) {
        float val = value;
        XPLMSetDatavf(prop->ref, &val, prop->index, 1);
        return 0;
    }

    if (xplmType_Data & type) {
        std::string s = toString(value);
        XPLMSetDatab(prop->ref, (void*)s.c_str(), 0, s.length() + 1);
        return 0;
    }

    return -2;
}


/// Returne value of property as float
static float getPropFloat(SaslPropRef property, int *err)
{
    if (err)
        *err = 0;

    Property *prop = (Property*)property;
    if (! prop) {
        if (err)
            *err = 1;
        return 0;
    }

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);

    if (xplmType_Float & type)
        return XPLMGetDataf(prop->ref);
    
    if (xplmType_Int & type)
        return XPLMGetDatai(prop->ref);
    
    if (xplmType_Double & type)
        return (float)XPLMGetDatad(prop->ref);

    if (xplmType_FloatArray & type) {
        float val = 0;
        XPLMGetDatavf(prop->ref, &val, prop->index, 1);
        return val;
    }

    if (xplmType_IntArray & type) {
        int val = 0;
        XPLMGetDatavi(prop->ref, &val, prop->index, 1);
        return val;
    }
    
    if (xplmType_Data & type) {
        int len = XPLMGetDatab(prop->ref, NULL, 0, 0);
        if (0 < len) {
#ifdef WINDOWS
            char *buf = (char*)alloca(len + 1);
#else
            char buf[len + 1];
#endif
            XPLMGetDatab(prop->ref, buf, 0, len);
            buf[len] = 0;
            return strToFloat(buf);
        } else
            return 0;
    }
    
    if (err)
        *err = 1;

    return 0;
}


/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
static int setPropFloat(SaslPropRef property, float value)
{
    Property *prop = (Property*)property;
    if (! prop)
        return -1;

    XPlaneProps *props = prop->parent;
    if (! props->initialized) 
        props->propsToSet.push_back(SetPropCmd(prop, value));

    if (! XPLMCanWriteDataRef(prop->ref))
        return -1;

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);

    if (xplmType_Float & type) {
        XPLMSetDataf(prop->ref, value);
        return 0;
    }
    
    if (xplmType_Int & type) {
        XPLMSetDatai(prop->ref, (int)value);
        return 0;
    }
    
    if (xplmType_Double & type) {
        XPLMSetDatad(prop->ref, (double)value);
        return 0;
    }

    if (xplmType_FloatArray & type) {
        XPLMSetDatavf(prop->ref, &value, prop->index, 1);
        return 0;
    }

    if (xplmType_IntArray & type) {
        int val = (int)value;
        XPLMSetDatavi(prop->ref, &val, prop->index, 1);
        return 0;
    }

    if (xplmType_Data & type) {
        std::string s = toString(value);
        XPLMSetDatab(prop->ref, (void*)s.c_str(), 0, s.length() + 1);
        return 0;
    }
    
    return -2;
}

/// Returne value of property as double
static double getPropDouble(SaslPropRef property, int *err)
{
    if (err)
        *err = 0;

    Property *prop = (Property*)property;
    if (! prop) {
        if (err)
            *err = 1;
        return 0;
    }

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);

    if (xplmType_Double & type)
        return (float)XPLMGetDatad(prop->ref);

    if (xplmType_Float & type)
        return XPLMGetDataf(prop->ref);
    
    if (xplmType_Int & type)
        return XPLMGetDatai(prop->ref);
    
    if (xplmType_FloatArray & type) {
        float val = 0;
        XPLMGetDatavf(prop->ref, &val, prop->index, 1);
        return val;
    }

    if (xplmType_IntArray & type) {
        int val = 0;
        XPLMGetDatavi(prop->ref, &val, prop->index, 1);
        return val;
    }
    
    if (xplmType_Data & type) {
        int len = XPLMGetDatab(prop->ref, NULL, 0, 0);
        if (0 < len) {
#ifdef WINDOWS
            char *buf = (char*)alloca(len + 1);
#else
            char buf[len + 1];
#endif
            XPLMGetDatab(prop->ref, buf, 0, len);
            buf[len] = 0;
            return strToDouble(buf);
        } else
            return 0;
    }
    
    if (err)
        *err = 1;

    return 0;
}


/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
static int setPropDouble(SaslPropRef property, double value)
{
    Property *prop = (Property*)property;
    if (! prop)
        return -1;
    
    XPlaneProps *props = prop->parent;
    if (! props->initialized) 
        props->propsToSet.push_back(SetPropCmd(prop, value));

    if (! XPLMCanWriteDataRef(prop->ref))
        return -1;

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);

    if (xplmType_Double & type) {
        XPLMSetDatad(prop->ref, value);
        return 0;
    }

    if (xplmType_Float & type) {
        XPLMSetDataf(prop->ref, (float)value);
        return 0;
    }
    
    if (xplmType_Int & type) {
        XPLMSetDatai(prop->ref, (int)value);
        return 0;
    }
    
    if (xplmType_FloatArray & type) {
        float val = (float)value;
        XPLMSetDatavf(prop->ref, &val, prop->index, 1);
        return 0;
    }

    if (xplmType_IntArray & type) {
        int val = (int)value;
        XPLMSetDatavi(prop->ref, &val, prop->index, 1);
        return 0;
    }

    if (xplmType_Data & type) {
        std::string s = toString(value);
        XPLMSetDatab(prop->ref, (void*)s.c_str(), 0, s.length() + 1);
        return 0;
    }
    
    return -2;
}


static int copyStr(char *dest, int maxSize, const std::string &src, int *err)
{
    int len = src.length();
    if (dest && maxSize) {
        int flen = len + 1;
        int toCopy = flen < maxSize ? flen : maxSize;
        memcpy(dest, src.c_str(), toCopy);
        dest[toCopy - 1] = 0;
        if ((flen < maxSize) && (err))
            *err = 1;
    }
    return src.length();
}


/// Returne value of property as string
/// returns length of string
static int getPropString(SaslPropRef property, char *buf, int maxSize, int *err)
{
    if (err)
        *err = 0;

    Property *prop = (Property*)property;
    if (! prop) {
        if (err)
            *err = 1;
        return 0;
    }

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);
    
    if (xplmType_Data & type) {
        int sz = XPLMGetDatab(prop->ref, NULL, 0, 0);
        if (buf) {
            int res = XPLMGetDatab(prop->ref, buf, 0, maxSize);
            if (res < maxSize)
                buf[res] = 0;
            else
                buf[maxSize - 1] = 0;
        }
        return sz;
    }
    
    if (xplmType_Double & type)
        return copyStr(buf, maxSize, toString(XPLMGetDatad(prop->ref)), err);

    if (xplmType_Float & type)
        return copyStr(buf, maxSize, toString(XPLMGetDataf(prop->ref)), err);
    
    if (xplmType_Int & type)
        return copyStr(buf, maxSize, toString(XPLMGetDatai(prop->ref)), err);
    
    if (xplmType_FloatArray & type) {
        float val = 0;
        XPLMGetDatavf(prop->ref, &val, prop->index, 1);
        return copyStr(buf, maxSize, toString(val), err);
    }

    if (xplmType_IntArray & type) {
        int val = 0;
        XPLMGetDatavi(prop->ref, &val, prop->index, 1);
        return copyStr(buf, maxSize, toString(val), err);
    }
    
    if (err)
        *err = 1;

    return 0;
}


/// Sets value of property as string
/// Returns zero on cuccess or non-zero on error
static int setPropString(SaslPropRef property, const char *value)
{
    Property *prop = (Property*)property;
    if ((! prop) || (! value))
        return -1;
    
    XPlaneProps *props = prop->parent;
    if (! props->initialized) 
        props->propsToSet.push_back(SetPropCmd(prop, value));

    if (! XPLMCanWriteDataRef(prop->ref))
        return -1;

    XPLMDataTypeID type = XPLMGetDataRefTypes(prop->ref);

    if (xplmType_Data & type) {
        XPLMSetDatab(prop->ref, (void*)value, 0, strlen(value) + 1);
        return 0;
    }

    if (xplmType_Double & type) {
        XPLMSetDatad(prop->ref, strToDouble(value));
        return 0;
    }

    if (xplmType_Float & type) {
        XPLMSetDataf(prop->ref, strToFloat(value));
        return 0;
    }
    
    if (xplmType_Int & type) {
        XPLMSetDatai(prop->ref, strToInt(value));
        return 0;
    }
    
    if (xplmType_FloatArray & type) {
        float val = strToFloat(value);
        XPLMSetDatavf(prop->ref, &val, prop->index, 1);
        return 0;
    }

    if (xplmType_IntArray & type) {
        int val = strToInt(value);
        XPLMSetDatavi(prop->ref, &val, prop->index, 1);
        return 0;
    }
    
    return -2;
}


/// Returns value of custom int property
static int readInt(void *refcon)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) 
        return p->data.intValue;
    else
        return 0;
}

/// Set value of custom int property
static void writeInt(void *refcon, int value)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) 
        p->data.intValue = value;
}

/// Create integer property
static SaslPropRef createIntProp(XPlaneProps *props, const char *name)
{
    CustomProperty *prop = new CustomProperty;
    prop->data.intValue = 0;
    prop->ref = XPLMRegisterDataAccessor(name, xplmType_Int, 1, 
            readInt, writeInt,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            prop, prop);
    if (! prop->ref) {
        delete prop;
        return NULL;
    }
    props->customProps[name] = prop;
    return getPropRef(props, name, PROP_INT);
}


/// Returns value of custom float property
static float readFloat(void *refcon)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) 
        return p->data.floatValue;
    else
        return 0;
}


/// Set value of custom int property
static void writeFloat(void *refcon, float value)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) 
        p->data.floatValue = value;
}

/// Create integer property
static SaslPropRef createFloatProp(XPlaneProps *props, const char *name)
{
    CustomProperty *prop = new CustomProperty;
    prop->data.floatValue = 0;
    prop->ref = XPLMRegisterDataAccessor(name, xplmType_Float, 1, 
            NULL, NULL, readFloat, writeFloat,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            prop, prop);
    if (! prop->ref) {
        delete prop;
        return NULL;
    }
    props->customProps[name] = prop;
    return getPropRef(props, name, PROP_FLOAT);
}


/// Returns value of custom double property
static double readDouble(void *refcon)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) 
        return p->data.doubleValue;
    else
        return 0;
}


/// Set value of custom int property
static void writeDouble(void *refcon, double value)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) 
        p->data.doubleValue = value;
}


/// Create integer property
static SaslPropRef createDoubleProp(XPlaneProps *props, const char *name)
{
    CustomProperty *prop = new CustomProperty;
    prop->data.doubleValue = 0;
    prop->ref = XPLMRegisterDataAccessor(name, xplmType_Double, 1, 
            NULL, NULL, NULL, NULL, readDouble, writeDouble,
            NULL, NULL, NULL, NULL, NULL, NULL,
            prop, prop);
    if (! prop->ref) {
        delete prop;
        return NULL;
    }
    props->customProps[name] = prop;
    return getPropRef(props, name, PROP_DOUBLE);
}


/// Returns value of custom string property
static int readString(void *refcon, void *value, int offset, int maxSize)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) {
        int len = p->data.stringValue.length();
        if (value) {
            int sz = len + 1 - offset;
            int realSz = sz > maxSize ? maxSize : sz;
            memcpy(value, p->data.stringValue.c_str() + offset, realSz);
            return realSz;
        } else
            return len + 1;
    } else
        return 0;
}


/// Set value of custom string property
static void writeString(void *refcon, void *value, int offset, int size)
{
    CustomProperty *p = (CustomProperty*)refcon;
    if (p) {
        if (! offset)
            p->data.stringValue = std::string((const char*)value, size - 1);
        else 
            p->data.stringValue = p->data.stringValue.substr(0, offset) + 
                std::string((const char*)value, size - 1);
    }
}



/// Create string property
static SaslPropRef createStringProp(XPlaneProps *props, const char *name, 
        int maxSize)
{
    CustomProperty *prop = new CustomProperty;
    prop->ref = XPLMRegisterDataAccessor(name, xplmType_Data, 1, 
            NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, readString, writeString,
            prop, prop);
    if (! prop->ref) {
        delete prop;
        return NULL;
    }
    props->customProps[name] = prop;
    return getPropRef(props, name, PROP_STRING);
}


/// Create new propery and returns reference to it.
/// If property already exists just returns reference to it.
static SaslPropRef createProp(SaslProps props, const char *name, int type, int maxSize)
{
    XPlaneProps *p = (XPlaneProps*)props;
    if (! (p && name))
        return NULL;
    
    CustomPropsMap::iterator i = p->customProps.find(name);
    if (i != p->customProps.end())
        return getPropRef(p, name, type);

    switch (type) {
        case PROP_INT: return createIntProp(p, name);
        case PROP_FLOAT: return createFloatProp(p, name);
        case PROP_DOUBLE: return createDoubleProp(p, name);
        case PROP_STRING: return createStringProp(p, name, maxSize);
    }

    return NULL;
}


/// delayed properties write
static int updateProps(SaslProps props)
{
    XPlaneProps *p = (XPlaneProps*)props;

    if (! p->initialized) {
        p->initialized = true;
        for (PropsToSet::iterator i = p->propsToSet.begin(); 
                i != p->propsToSet.end(); i++)
        {
            SetPropCmd &v = *i;
            switch (v.type) {
                case PROP_INT: setPropInt(v.property, v.data.intValue); break;
                case PROP_FLOAT: setPropFloat(v.property, v.data.floatValue); break;
                case PROP_DOUBLE: setPropDouble(v.property, v.data.doubleValue); break;
                case PROP_STRING: setPropString(v.property, v.data.stringValue.c_str()); break;
            }
        }
        p->propsToSet.clear();
    }

    return 0;
}


/// Returns value of int callback property
static int getIntCallback(void *refcon)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->getter) {
        int value;
        p->getter(PROP_INT, &value, sizeof(value), p->data);
        return value;
    } else
        return 0;
}

/// Set value of int callback property
static void setIntCallback(void *refcon, int value)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->setter)
        p->setter(PROP_INT, &value, sizeof(value), p->data);
}

/// Returns value of float callback property
static float getFloatCallback(void *refcon)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->getter) {
        float value;
        p->getter(PROP_FLOAT, &value, sizeof(value), p->data);
        return value;
    } else
        return 0;
}

/// Set value of float callback property
static void setFloatCallback(void *refcon, float value)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->setter)
        p->setter(PROP_FLOAT, &value, sizeof(value), p->data);
}

/// Returns value of double callback property
static double getDoubleCallback(void *refcon)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->getter) {
        double value;
        p->getter(PROP_DOUBLE, &value, sizeof(value), p->data);
        return value;
    } else
        return 0;
}

/// Set value of double callback property
static void setDoubleCallback(void *refcon, double value)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->setter)
        p->setter(PROP_DOUBLE, &value, sizeof(value), p->data);
}


/// Returns value of string callback property
static int getStringCallback(void *refcon, void *buf, int offset, int maxLen)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->getter) {
        if ((! offset) || (! buf))
            return p->getter(PROP_STRING, buf, maxLen, p->data);
        else {
            int len = p->getter(PROP_STRING, NULL, 0, p->data);
            if (len) {
#ifdef WINDOWS
                void *tbuf = alloca(len);
#else
                void *tbuf[len];
#endif
                p->getter(PROP_STRING, tbuf, len, p->data);
                int realSz = len - offset;
                if (realSz > maxLen)
                    realSz = maxLen;
                memcpy(buf, tbuf, realSz);
                return realSz;
            } else
                return 0;
        }
    } else
        return 0;
}

/// Set value of double callback property
static void setStringCallback(void *refcon, void *value, int offset, int len)
{
    FuncProperty *p = (FuncProperty*)refcon;
    if (p && p->setter) {
        if (! offset)
            p->setter(PROP_STRING, value, len, p->data);
        else {
            int newSize = offset + len;
            int oldSize = p->getter(PROP_STRING, NULL, 0, p->data);
            int bufSize = newSize > oldSize ? newSize : oldSize;
#ifdef WINDOWS
            char *buf = (char*)alloca(bufSize);
#else
            void *buf[bufSize];
#endif
            p->getter(PROP_STRING, buf, bufSize, p->data);
            memcpy(buf + offset, value, len);
            p->setter(PROP_STRING, &value, bufSize, p->data);
        }
    }
}


// create functional property
static SaslPropRef createFuncProp(SaslProps props, const char *name, 
            int type, int size, sasl_prop_getter_callback getter, 
            sasl_prop_setter_callback setter, void *ref)
{
    XPlaneProps *p = (XPlaneProps*)props;

    SaslPropRef propRef = getPropRef(props, name, type);
    if (propRef)
        return propRef;

    FuncProperty *funcProp = new FuncProperty;
    funcProp->data = ref;
    funcProp->getter = getter;
    funcProp->setter = setter;

    switch (type) {
        case PROP_INT:
            funcProp->ref = XPLMRegisterDataAccessor(name, xplmType_Int, 1,
                    getIntCallback, setIntCallback, NULL, NULL, NULL, NULL,
                    NULL, NULL, NULL, NULL, NULL, NULL, funcProp, funcProp);
            break;
        case PROP_FLOAT:
            funcProp->ref = XPLMRegisterDataAccessor(name, xplmType_Float, 1,
                    NULL, NULL, getFloatCallback, setFloatCallback, NULL, NULL,
                    NULL, NULL, NULL, NULL, NULL, NULL, funcProp, funcProp);
            break;
        case PROP_DOUBLE:
            funcProp->ref = XPLMRegisterDataAccessor(name, xplmType_Double, 1,
                    NULL, NULL, NULL, NULL, getDoubleCallback, setDoubleCallback,
                    NULL, NULL, NULL, NULL, NULL, NULL, funcProp, funcProp);
            break;
        case PROP_STRING:
            funcProp->ref = XPLMRegisterDataAccessor(name, xplmType_Data, 1,
                    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                    getStringCallback, setStringCallback, funcProp, funcProp);
            break;
    }

    p->funcProps.push_back(funcProp);

    return getPropRef(props, name, type);
}


static SaslPropsCallbacks callbacks = { getPropRef, freePropRef, createProp, 
        createFuncProp, getPropInt, setPropInt, getPropFloat, 
        setPropFloat, getPropDouble, setPropDouble, getPropString,
        setPropString, updateProps, NULL };


SaslPropsCallbacks* xap::getPropsCallbacks()
{
    return &callbacks;
}

