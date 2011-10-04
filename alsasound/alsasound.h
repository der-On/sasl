#ifndef __SOUND_H__
#define __SOUND_H__


#if defined(__cplusplus)
extern "C" {
#endif


#include "libavionics.h"


// sound engine handler
struct SaslAlsaSound;


// initialize sound engine
SaslAlsaSound* sasl_init_alsa_sound(SASL sasl);

// destroy sound engine
void sasl_done_alsa_sound(SaslAlsaSound *sound);

#if defined(__cplusplus)
};
#endif

#endif

