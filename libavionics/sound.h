#ifndef __SOUND_H__
#define __SOUND_H__


#include "luna.h"
#include "libavcallbacks.h"

namespace xa {


/// wrapper for underlaying callbacks
class Sound
{
    private:
        /// sound functions
        SaslSoundCallbacks *sound;

    public:
        /// create sound functions wrapper
        Sound();

    public:
        /// Setup sound callbacks
        /// \param callbacks sound engine callbacks.
        void setCallbacks(SaslSoundCallbacks *callbacks);

        /// Load sample into memory.  Returns sample handler or 0 if can't load sample
        /// \param fileName path to sample on disk
        int loadSample(const char *fileName);
        
        /// Unload sample to free memory
        /// \param sampleId sample handler
        void unloadSample(int sampleId);

        // Play loaded sample
        /// \param sampleId sample handler
        /// \param loop if non-zero sound will be looped
        void playSample(int sampleId, int loop);

        /// Stop playing sample
        /// \param sampleId sample handler
        void stopSample(int sampleId);

        /// Set gain of sample
        /// \param sampleId sample handler
        /// \param gain gain ratio from 0 to 1000
        void setSampleGain(int sampleId, int gain);

        /// Set pitch of sample
        /// \param sampleId sample handler
        /// \param pitch pitch ration from 0 to 1000
        void setSamplePitch(int sampleId, int pitch);

        /// Rewind sample to beginning
        /// \param sampleId sample handler
        void rewindSample(int sampleId);

        /// Set environment to which sound belongs
        /// \param sampleId sample handler
        /// \param env equals to SOUND_INTERNAL, SOUND_EXTERNAL or SOUND_EVERYWHERE
        void setSampleEnv(int sampleId, int env);
        
        /// Retuns environment to which sound belongs.  See sampleSetEnv
        /// \param sampleId sample handler
        int getSampleEnv(int sampleId);

        /// Set position of sample
        /// \param sampleId sample handler
        /// \param x sound position x coord
        /// \param y sound position y coord
        /// \param z sound position z coord
        void setSamplePosition(int sampleId, float x, float y, float z);

        /// Get position of sample
        /// \param sampleId sample handler
        /// \param x sound position x coord
        /// \param y sound position y coord
        /// \param z sound position z coord
        void getSamplePosition(int sampleId, float &x, float &y, float &z);

        /// Set direction of sample.  Sets both heading and up vectors.
        /// \param sampleId sample handler
        /// \param x sound heading vector x coord
        /// \param y sound heading vector y coord
        /// \param z sound heading vector z coord
        void setSampleDirection(int sampleId, float x, float y, float z);

        /// Get direction of sample.  Sets both heading and up vectors.
        /// \param sampleId sample handler
        /// \param x sound heading vector x coord
        /// \param y sound heading vector y coord
        /// \param z sound heading vector z coord
        void getSampleDirection(int sampleId, float &x, float &y, float &z);
        
        /// Set maximum distance of sample
        /// \param sampleId sample handler
        /// \param maxDistance maximum distance of sample
        void setSampleMaxDistance(int sampleId, float maxDistance);
        
        /// Set sample rolloff factor
        /// \param sampleId sample handler
        /// \param rolloff sample rolloff factor
        void setSampleRolloff(int sampleId, float rolloff);

        /// Set distance at which sample was recorded
        /// \param sampleId sample handler
        /// \param distance reference distance
        void setSampleRefDistance(int sampleId, float distance);

        /// Set sample cone parameters
        /// \param sampleId sample handler
        /// \param outerGain outer gain
        /// \param innerAngle inner cone angle
        /// \param outerAngle outer cone angle
        void setSampleCone(int sampleId, float outerGain, 
                float innerAngle, float outerAngle);

        /// Get sample cone parameters
        /// \param sampleId sample handler
        /// \param outerGain outer gain
        /// \param innerAngle inner cone angle
        /// \param outerAngle outer cone angle
        void getSampleCone(int sampleId, float &outerGain, 
                float &innerAngle, float &outerAngle);
        
        /// Set sample relative of listener
        /// \param sampleId sample handler
        /// \param relative if true sample position is relative of camera
        void setSampleRelative(int sampleId, bool relative);
        
        /// Returns true if sample position specified relative of listener
        /// \param sampleId sample handler
        bool getSampleRelative(int sampleId);


        /// Set current playback environment
        /// \param env equals to SOUND_INTERNAL or SOUND_EXTERNAL
        void setListenerEnv(int env);
        
        /// Set position of listener
        /// \param x listener position x coord
        /// \param y listener position y coord
        /// \param z listener position z coord
        void setListenerPosition(float x, float y, float z);

        /// Get position of listener
        /// \param x listener position x coord
        /// \param y listener position y coord
        /// \param z listener position z coord
        void getListenerPosition(float &x, float &y, float &z);

        /// Set orientation of listener.  Sets both heading and up vectors.
        /// \param x listener heading vector x coord
        /// \param y listener heading vector y coord
        /// \param z listener heading vector z coord
        /// \param ux listener up vector x coord
        /// \param uy listener up vector y coord
        /// \param uz listener up vector z coord
        void setListenerOrientation(float x, float y, float z,
                float ux, float uy, float uz);

        /// Get orientation of listener.  Sets both heading and up vectors.
        /// \param x listener heading vector x coord
        /// \param y listener heading vector y coord
        /// \param z listener heading vector z coord
        /// \param ux listener up vector x coord
        /// \param uy listener up vector y coord
        /// \param uz listener up vector z coord
        void getListenerOrientation(float &x, float &y, float &z,
                float &ux, float &uy, float &uz);

        /// Returns true if sample playing now
        /// \param sampleId sample handler
        bool isSamplePlaying(int sampleId);

        /// Set gain for all samles
        /// \param gain gain ratio from 0 to 1000
        void setMasterGain(int gain);

        /// Allow sound engine do its job
        /// Call it on every frame
        void update();

        /// Register functions in Lua
        void exportSoundToLua(Luna &lua);

};

};



#endif

