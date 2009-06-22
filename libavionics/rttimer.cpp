#include <sys/time.h>
#include <stdio.h>
#include "rttimer.h"

using namespace xa;

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

