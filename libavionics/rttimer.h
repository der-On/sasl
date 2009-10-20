#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef _MSC_VER
#include <time.h>
#else
#include <sys/time.h>
#endif


namespace xa {


/// Counts time in milliseconds since tmer start
class RtTimer
{
    private:
        time_t startSeconds;

    public:
        RtTimer();

    public:
        /// Returns number of milliseconds passed from timer creation
        long getTime();
};


};


#endif

