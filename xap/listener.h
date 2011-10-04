#ifndef __LISTENER_H__
#define __LISTENER_H__

#include "libavionics.h"


namespace xap {

// initialize tracking properties
void initListenerTracking();

// update position of listener
void updateListenerPosition(SASL sasl);

};

#endif

