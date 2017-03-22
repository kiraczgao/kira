#ifndef TIMERTESTING_H
#define TIMERTESTING_H
#include <sys/time.h>

class TimerTesting
{
public:
    TimerTesting();

public:
    void PrintlnTimeUse();

protected:
    timeval starttime;
    timeval endtime;
};

#endif // TIMERTESTING_H
