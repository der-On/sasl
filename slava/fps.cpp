#include "fps.h"


/// Period for FPS calculation in milliseconds
#define FPS_CALC_CYCLE 1000


Fps::Fps()
{
    countStartTime = 0;
    frames = 0;
    lastFps = 0;
    targetFps = 0;
    delay = 0;
}


void Fps::update()
{
    Uint32 now = SDL_GetTicks();

    if (! countStartTime) {
        countStartTime = now;
        frames = 0;
        return;
    }

    frames++;

    if (now >= countStartTime + FPS_CALC_CYCLE) {
        double fps = frames / ((now - countStartTime) / 1000.0);

        if (targetFps) {
            // make corrections to sleep time
            if (fps > targetFps)
                delay++;
            else if ((fps < targetFps) && (0 < delay))
                delay--;
        }

        countStartTime = now;
        frames = 0;
        lastFps = fps;
    }

    if (targetFps)
        SDL_Delay(delay);
}


