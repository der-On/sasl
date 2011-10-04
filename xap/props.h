#ifndef __PROPS_H__
#define __PROPS_H__

#include "libavionics.h"

namespace xap {

/// Returns pointer to XAvionics callbacks structure
struct SaslPropsCallbacks* getPropsCallbacks();

/// Initialize properties system
SaslProps propsInit();

/// Free properties structure
void propsDone(SaslProps props);

/// Free func properties only
void funcPropsDone(SaslProps props);

};

#endif

