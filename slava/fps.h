#ifndef __FPS_H__
#define __FPS_H__


#include <SDL.h>


///  FPs calculator
class Fps
{
    private:
        /// time of FPS calculation cycle start
        Uint32 countStartTime;

        /// Number of frames counted since countStartTime
        int frames;

        /// latest known FPS
        double lastFps;

        /// desired FPS or 0 if unlimited
        int targetFps;

        /// sleep time neccessary to keep target FPS
        Uint32 delay;

    public:
        Fps();

    public:
        /// update FPS counter and delay next frame if needed
        void update();

        /// Limit FPS
        void setTargetFps(int fps) { targetFps = fps; };
};


#endif

