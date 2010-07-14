#ifndef __PROPS_H__
#define __PROPS_H__

#include "xavionics.h"

namespace xap {

/// Returns pointer to XAvionics callbacks structure
struct PropsCallbacks* getPropsCallbacks();

/// Initialize properties system
Props propsInit();

/// Free properties structure
void propsDone(Props props);

/// Free func properties only
void funcPropsDone(Props props);
};

#endif

