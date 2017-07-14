#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "types.h"

struct timeval
{
    long tv_sec;
    long tv_usec;
};

struct timespec
{
    mytime_t tv_sec;
    long tv_nsec;
};

#endif  // SYS_TIME_H
