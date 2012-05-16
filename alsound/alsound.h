#ifndef __SOUND_H__
#define __SOUND_H__


#if defined(__cplusplus)
extern "C" {
#endif


#include "libavionics.h"


// sound engine handler
struct SaslAlSound;


// initialize sound engine
SaslAlSound* sasl_init_al_sound(SASL sasl);

// destroy sound engine
void sasl_done_al_sound(SaslAlSound *sound);

#if defined(__cplusplus)
};
#endif

#endif

