#include <list>
#include <map>
#include <string>
#include <stdlib.h>

#include "props.h"
#include "xavionics.h"

#include "xpsdk.h"


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


/// Self-created property
struct CustomProperty
{
    /// Reference to property for unregistering
    XPLMDataRef ref;

    /// Property data
    union {
        /// Property as int
        int intValue;

        /// Property as float
        float floatValue;

        /// Property as double
        double doubleValue;
    } data;
};


/// List of referneces to properties
typedef std::list<Property*> PropsList;


/// List of self-created properties
typedef std::map<std::string, CustomProperty*> CustomPropsMap;


/// X-Plane properties info
struct XPlaneProps {
    /// References to properties
    PropsList props;
    CustomPropsMap customProps;
};


/// Initialize properties structure
Props xap::propsInit()
{
    return new XPlaneProps;
}


/// Free properties structure
void xap::propsDone(Props props)
{
    XPlaneProps *p = (XPlaneProps*)props;
    if (! p)
        return;

    for (PropsList::iterator i = p->props.begin(); i != p->props.end(); ++i)
        delete *i;
    
    for (CustomPropsMap::iterator i = p->customProps.begin(); 
            i != p->customProps.end(); ++i)
    {
        XPLMUnregisterDataAccessor((*i).second->ref);
        delete (*i).second;
    }

    if (p)
        delete p;
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
static PropRef getPropRef(Props props, const char *name, int type)
{
    XPlaneProps *p = (XPlaneProps*)props;
    if (! (p && name))
        return NULL;

    int index = 0;
    std::string realName = name;
    cutArrayIndex(realName, index);

    XPLMDataRef ref = XPLMFindDataRef(realName.c_str());
    if (! ref)
        return NULL;

    Property *prop = new Property;
    prop->ref = ref;
    prop->index = index;
    prop->parent = p;
    p->props.push_back(prop);
    return prop;
}


/// Destroy unneeded reference to pointer
static void freePropRef(PropRef property)
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
static int getPropInt(PropRef property, int *err)
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

    if (err)
        *err = 1;

    return 0;
}


/// Sets value of property as integer
/// Returns zero on cuccess or non-zero on error
static int setPropInt(PropRef property, int value)
{
    Property *prop = (Property*)property;
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

    return -2;
}


/// Returne value of property as float
static float getPropFloat(PropRef property, int *err)
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
    
    if (err)
        *err = 1;

    return 0;
}


/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
static int setPropFloat(PropRef property, float value)
{
    Property *prop = (Property*)property;
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
    
    return -2;
}

/// Returne value of property as double
static double getPropDouble(PropRef property, int *err)
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
    
    if (err)
        *err = 1;

    return 0;
}


/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
static int setPropDouble(PropRef property, double value)
{
    Property *prop = (Property*)property;
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
static PropRef createIntProp(XPlaneProps *props, const char *name)
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
static PropRef createFloatProp(XPlaneProps *props, const char *name)
{
    CustomProperty *prop = new CustomProperty;
    prop->data.intValue = 0;
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
static PropRef createDoubleProp(XPlaneProps *props, const char *name)
{
    CustomProperty *prop = new CustomProperty;
    prop->data.intValue = 0;
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

/// Create new propery and returns reference to it.
/// If property already exists just returns reference to it.
static PropRef createProp(Props props, const char *name, int type)
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
    }

    return NULL;
}


static PropsCallbacks callbacks = { getPropRef, freePropRef, createProp, 
        getPropInt, setPropInt, getPropFloat, 
        setPropFloat, getPropDouble, setPropDouble, NULL, NULL };


PropsCallbacks* xap::getPropsCallbacks()
{
    return &callbacks;
}

