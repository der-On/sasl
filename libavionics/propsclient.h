#ifndef __PROPS_CLIENT_H__
#define __PROPS_CLIENT_H__


#include "libavionics.h"
#include "log.h"


namespace xa {

int connectToServer(SASL sasl, Log &log, const char *host, int port, 
        const char *secret);

};

#endif

