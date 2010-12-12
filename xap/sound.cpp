#include "sound.h"

#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>

#if APL
#include <Carbon/Carbon.h>
#endif

#include <AL/al.h>
#include <AL/alc.h>

#include "xpsdk.h"


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


/**************************************************************************************************************
 * WAVE FILE LOADING
 
**************************************************************************************************************/

// You can just use alutCreateBufferFromFile to load a wave file, but there seems to be a lot of problems with 
// alut not beign available, being deprecated, etc.  So...here's a stupid routine to load a wave file.  I have
// tested this only on x86 machines, so if you find a bug on PPC please let me know.

// Macros to swap endian-values.

#define SWAP_32(value)                 \
        (((((unsigned short)value)<<8) & 0xFF00)   | \
         ((((unsigned short)value)>>8) & 0x00FF))

#define SWAP_16(value)                     \
        (((((unsigned int)value)<<24) & 0xFF000000)  | \
         ((((unsigned int)value)<< 8) & 0x00FF0000)  | \
         ((((unsigned int)value)>> 8) & 0x0000FF00)  | \
         ((((unsigned int)value)>>24) & 0x000000FF))

// Wave files are RIFF files, which are "chunky" - each section has an ID and a length.  This lets us skip
// things we can't understand to find the parts we want.  This header is common to all RIFF chunks.
struct chunk_header { 
	int			id;
	int			size;
};

// WAVE file format info.  We pass this through to OpenAL so we can support mono/stereo, 8/16/bit, etc.
struct format_info {
	short		format;				// PCM = 1, not sure what other values are legal.
	short		num_channels;
        int		sample_rate;
	int		byte_rate;
	short		block_align;
	short		bits_per_sample;
};

// This utility returns the start of data for a chunk given a range of bytes it might be within.  Pass 1 for
// swapped if the machine is not the same endian as the file.
static char *	find_chunk(char * file_begin, char * file_end, int 
desired_id, int swapped)
{
	while(file_begin < file_end)
	{
		chunk_header * h = (chunk_header *) file_begin;
		if(h->id == desired_id && !swapped)
			return file_begin+sizeof(chunk_header);
		if(h->id == SWAP_32(desired_id) && swapped)
			return file_begin+sizeof(chunk_header);
		int chunk_size = swapped ? SWAP_32(h->size) : h->size;
		char * next = file_begin + chunk_size + sizeof(chunk_header);
		if(next > file_end || next <= file_begin)
			return NULL;
		file_begin = next;		
	}
	return NULL;
}

// Given a chunk, find its end by going back to the header.
static char * chunk_end(char * chunk_start, int swapped)
{
	chunk_header * h = (chunk_header *) (chunk_start - sizeof(chunk_header));
	return chunk_start + (swapped ? SWAP_32(h->size) : h->size);
}

#define FAIL(X) { XPLMDebugString(X); free(mem); return 0; }

#define RIFF_ID 0x46464952			// 'RIFF'
#define FMT_ID  0x20746D66			// 'FMT '
#define DATA_ID 0x61746164			// 'DATA'


static ALuint load_wave(const char * file_name)
{
	// First: we open the file and copy it into a single large memory buffer for processing.

	FILE * fi = fopen(file_name,"rb");
	if(fi == NULL)
	{
		XPLMDebugString("WAVE file load failed - could not open.\n");	
		return 0;
	}
	fseek(fi,0,SEEK_END);
	int file_size = ftell(fi);
	fseek(fi,0,SEEK_SET);
	char * mem = (char*) malloc(file_size);
	if(mem == NULL)
	{
		XPLMDebugString("WAVE file load failed - could not allocate memory.\n");
		fclose(fi);
		return 0;
	}
	if ((int)fread(mem, 1, file_size, fi) != file_size)
	{
		XPLMDebugString("WAVE file load failed - could not read file.\n");	
		free(mem);
		fclose(fi);
		return 0;
	}
	fclose(fi);
	char * mem_end = mem + file_size;
	
	// Second: find the RIFF chunk.  Note that by searching for RIFF both normal
	// and reversed, we can automatically determine the endian swap situation for
	// this file regardless of what machine we are on.
	
	int swapped = 0;
	char * riff = find_chunk(mem, mem_end, RIFF_ID, 0);
	if(riff == NULL)
	{
		riff = find_chunk(mem, mem_end, RIFF_ID, 1);
		if(riff)
			swapped = 1;
		else
			FAIL("Could not find RIFF chunk in wave file.\n")
	}
	
	// The wave chunk isn't really a chunk at all. :-(  It's just a "WAVE" tag 
	// followed by more chunks.  This strikes me as totally inconsistent, but
	// anyway, confirm the WAVE ID and move on.
	
	if (riff[0] != 'W' ||
		riff[1] != 'A' ||
		riff[2] != 'V' ||
		riff[3] != 'E')
		FAIL("Could not find WAVE signature in wave file.\n")

	char * format = find_chunk(riff+4, chunk_end(riff,swapped), FMT_ID, swapped);
	if(format == NULL)
		FAIL("Could not find FMT  chunk in wave file.\n")
	
	// Find the format chunk, and swap the values if needed.  This gives us our real format.
	
	format_info * fmt = (format_info *) format;
	if(swapped)
	{
		fmt->format = SWAP_16(fmt->format);
		fmt->num_channels = SWAP_16(fmt->num_channels);
		fmt->sample_rate = SWAP_32(fmt->sample_rate);
		fmt->byte_rate = SWAP_32(fmt->byte_rate);
		fmt->block_align = SWAP_16(fmt->block_align);
		fmt->bits_per_sample = SWAP_16(fmt->bits_per_sample);
	}
	
	// Reject things we don't understand...expand this code to support weirder audio formats.
	
	if(fmt->format != 1) FAIL("Wave file is not PCM format data.\n")
	if(fmt->num_channels != 1 && fmt->num_channels != 2) FAIL("Must have mono or stereo sound.\n")
	if(fmt->bits_per_sample != 8 && fmt->bits_per_sample != 16) FAIL("Must have 8 or 16 bit sounds.\n")
	char * data = find_chunk(riff+4, chunk_end(riff,swapped), DATA_ID, swapped) ;
	if(data == NULL)
		FAIL("I could not find the DATA chunk.\n")
	
	int sample_size = fmt->num_channels * fmt->bits_per_sample / 8;
	int data_bytes = chunk_end(data,swapped) - data;
	int data_samples = data_bytes / sample_size;
	
	// If the file is swapped and we have 16-bit audio, we need to endian-swap the audio too or we'll 
	// get something that sounds just astoundingly bad!
	
	if(fmt->bits_per_sample == 16 && swapped)
	{
		short * ptr = (short *) data;
		int words = data_samples * fmt->num_channels;
		while(words--)
		{
			*ptr = SWAP_16(*ptr);
			++ptr;
		}
	}
	
	// Finally, the OpenAL crud.  Build a new OpenAL buffer and send the data to OpenAL, passing in
	// OpenAL format enums based on the format chunk.
	
	ALuint buf_id = 0;
	alGenBuffers(1, &buf_id);
	if(buf_id == 0) FAIL("Could not generate buffer id.\n");
	
	alBufferData(buf_id, fmt->bits_per_sample == 16 ?  
                (fmt->num_channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16) : 
                (fmt->num_channels == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8), 
                data, data_bytes, fmt->sample_rate);
	free(mem);
	return buf_id;
}




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
        buffer = load_wave(fileName);
        if (! buffer) {
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

    if (alcGetCurrentContext() == sound.context)
        alcMakeContextCurrent(NULL);
    alcDestroyContext(sound.context);
    alcCloseDevice(sound.device);
    
    xa_set_sound_engine(xa, NULL, NULL);
}

