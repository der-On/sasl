#include "sound.h"

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#if APL
#include <Carbon/Carbon.h>
#endif

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>


namespace xap {

// changes context to specified and restores old context after destruction
class ContextChanger
{
    private:
        // saved old context
        ALCcontext *oldContext;

    public:
        // save old context and change to our context
        ContextChanger(ALCcontext *context) {
            oldContext = alcGetCurrentContext();
            alcMakeContextCurrent(context);
            alGetError();
            alutGetError();
        };

        // restore saved context
        ~ContextChanger() {
            alcMakeContextCurrent(oldContext);
        }
};

// sound parameters
struct Sound
{
    // xap sound api callbacks
    XaSoundCallbacks callbacks;

    // OpenAL device
    ALCdevice *device;

    // OpenAL context
    ALCcontext *context;

    // loaded sound files
    std::map<std::string, ALuint> buffers;

    // sources associated with sound files
    std::vector<ALuint> sources;
};

};

using namespace xap;

/// sound API
static Sound sound;


// load sound from disk or reuses already loaded sound
static int loadSound(XaSoundCallbacks *callbacks, const char *fileName)
{
    if ((! fileName) || (! sound.device) || (! sound.context))
        return 0;
    
    ContextChanger changer(sound.context);

    ALuint buffer = 0;
    std::map<std::string, ALuint>::const_iterator i = 
        sound.buffers.find(fileName);
    if (i == sound.buffers.end()) {
        buffer = alutCreateBufferFromFile(fileName);
        if ((AL_NONE == buffer) || (AL_NO_ERROR != alGetError())) {
            printf("XAP: Can't load sound '%s'\n", fileName);
            return 0;
        }
        sound.buffers[fileName] = buffer;
    } else {
        buffer = (*i).second;
    }

    ALuint source;
    alGenSources(1, &source);
    if (AL_NO_ERROR != alGetError()) {
        printf("XAP: Can't create source\n");
        return 0;
    }

    ALfloat zero[] = { 0, 0, 0 };

    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSourcefv(source, AL_POSITION, zero);
    alSourcefv(source, AL_VELOCITY, zero);
    alSourcei(source, AL_LOOPING, 0);

    if (AL_NO_ERROR != alGetError()) {
        printf("XAP: Can't set source properties\n");
        alDeleteSources(1, &source);
        return 0;
    }

    sound.sources.push_back(source);
    return sound.sources.size();
}


// play sound source
static void playSound(struct XaSoundCallbacks *s, int sampleId, int loop)
{
    if ((! sound.device) || (! sound.context) || (! sampleId) ||
            (sound.sources.size() < (unsigned)sampleId))
        return;
    
    ContextChanger changer(sound.context);

    ALuint source = sound.sources[sampleId - 1];
    
    ALfloat zero[] = { 0, 0, 0 };
    ALfloat orient[] = { 0, 0, -1,  0, 1, 0 };

    alSourcei(source, AL_LOOPING, loop);
    alListenerfv(AL_POSITION, zero);
    alListenerfv(AL_VELOCITY, zero);
    alListenerfv(AL_ORIENTATION, orient);

    ALuint err = alGetError();
    if (AL_NO_ERROR != err)
        printf("setup error!!!! %i\n", err);
    alSourcePlay(source);

    err = alGetError();
    if (AL_NO_ERROR != err)
        printf("play error!!!! %i\n", err);
}


// stop sample playing
static void stopSound(struct XaSoundCallbacks *s, int sampleId)
{
    if ((! sound.device) || (! sound.context) || (! sampleId) ||
            (sound.sources.size() < (unsigned)sampleId))
        return;
    
    ContextChanger changer(sound.context);

    ALuint source = sound.sources[sampleId - 1];
    alSourceStop(source);
}


// set gain of sound
static void setGain(struct XaSoundCallbacks *s, int sampleId, int gain)
{
    if ((! sound.device) || (! sound.context) || (! sampleId) ||
            (sound.sources.size() < (unsigned)sampleId))
        return;
    
    ContextChanger changer(sound.context);

    ALuint source = sound.sources[sampleId - 1];
    alSourcef(source, AL_GAIN, (float)gain / 1000.0);
}


// set pitch of sound
static void setPitch(struct XaSoundCallbacks *s, int sampleId, int pitch)
{
    if ((! sound.device) || (! sound.context) || (! sampleId) ||
            (sound.sources.size() < (unsigned)sampleId))
        return;
    
    ContextChanger changer(sound.context);

    ALuint source = sound.sources[sampleId - 1];
    alSourcef(source, AL_PITCH, (float)pitch / 1000.0);
}


/// Rewind sample to beginning
static void rewindSound(struct XaSoundCallbacks *s, int sampleId)
{
    if ((! sound.device) || (! sound.context) || (! sampleId) ||
            (sound.sources.size() < (unsigned)sampleId))
        return;
    
    ContextChanger changer(sound.context);

    ALuint source = sound.sources[sampleId - 1];
    alSourceRewind(source);
}


// initialize sound engined
void xap::initSound(XA xa)
{
    struct XaSoundCallbacks cb = { loadSound, playSound, stopSound, setGain, 
        setPitch, rewindSound };
    sound.callbacks = cb;
    sound.device = alcOpenDevice(NULL);
    sound.context = alcCreateContext(sound.device, NULL);

    // some magic
    alGetError();
    alutInitWithoutContext(NULL, 0);
    alutGetError();

    xa_set_sound_engine(xa, &sound.callbacks, NULL);
}


// destroy sound engine
void xap::doneSound(XA xa)
{
    if ((! sound.device) || (! sound.context))
        return;
    
    {
        ContextChanger(sound.context);
        
        for (std::vector<ALuint>::iterator i = sound.sources.begin(); 
                i != sound.sources.end(); i++)
        {
            ALuint source = (*i);
            alDeleteSources(1, &source);
        }
        sound.sources.clear();
        
        for (std::map<std::string, ALuint>::iterator i = sound.buffers.begin(); 
                i != sound.buffers.end(); i++)
        {
            ALuint buffer = (*i).second;
            alDeleteBuffers(1, &buffer);
        }
        sound.buffers.clear();
    }

    alutExit();

    if (alcGetCurrentContext() == sound.context)
        alcMakeContextCurrent(NULL);
    alcDestroyContext(sound.context);
    alcCloseDevice(sound.device);
    
    xa_set_sound_engine(xa, NULL, NULL);
}

