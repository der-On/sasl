#include "rttimer.h"
#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <stdlib.h>

using namespace xa;

#ifdef _MSC_VER
RtTimer::RtTimer()
{
    startSeconds = GetTickCount();
}

long RtTimer::getTime()
{
    return GetTickCount() - startSeconds;
}
#else
RtTimer::RtTimer()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    startSeconds = tv.tv_sec;
}

long RtTimer::getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int seconds = tv.tv_sec - startSeconds;
    return seconds * 1000 + tv.tv_usec / 1000;
}
#endif

