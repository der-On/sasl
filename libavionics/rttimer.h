#ifndef __TIMER_H__
#define __TIMER_H__
#include <sys/time.h>


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

