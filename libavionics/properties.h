#ifndef __PROPS_H__
#define __PROPS_H__


#include "xcallbacks.h"
#include <string>
#include "luna.h"


namespace xa {


/// Access to properties
class Properties
{
    private:
        /// Pluggable properties module callbacks
        struct PropsCallbacks *propsCallbacks;

        /// Pointer to properties subsystem
        Props props;

        /// If equals to false, properties functions will throw 
        // exceptions on errors
        bool ignorePropsErrors;

    public:
        Properties();

        ~Properties();

    public:
        /// Enable or disable throwing exceptions on properties
        /// errors.  Without exceptions you can still get error status
        /// by passing pointer to error status variable in getProp* functions.
        void setIgnorePropsErrors(bool ignore) { ignorePropsErrors = true; }

        /// Setup properties callbacks
        /// \param callbacks structure full of properties callbacks
        /// \param props properties handler
        void setProps(struct PropsCallbacks *callbacks, Props props);

        /// Returns pointer to property with specified name or NULL if not found
        PropRef getProp(const std::string &name, int type);

        /// Create new property
        PropRef createProp(const std::string &name, int type);

        /// Release property struture
        void freeProp(PropRef prop);

        /// Returns value of property as integer
        /// On errors returns dflt
        int getPropi(PropRef prop, int dflt=0, int *err=NULL);

        /// Set value of integer property.
        int setProp(PropRef prop, int value);
        
        /// Returns value of property as integer
        /// On errors returns dflt
        float getPropf(PropRef prop, float dflt=0, int *err=NULL);
        
        /// Set value of float property.
        int setProp(PropRef prop, float value);
        
        /// Returns value of property as double
        /// On errors returns dflt
        float getPropd(PropRef prop, double dflt=0, int *err=NULL);
        
        /// Set value of double property.
        int setProp(PropRef prop, double value);

        /// Update properties subsystem
        int update();
};


/// Register properties in Lua
void exportPropsToLua(Luna &lua);

};


#endif

