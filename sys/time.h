#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "time.h"

struct timeval
{
    long tv_sec;
    long tv_usec;
};

struct timespec
{
    time_t tv_sec;
    long tv_nsec;
};

#endif  // SYS_TIME_H
