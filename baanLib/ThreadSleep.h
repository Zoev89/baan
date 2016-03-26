#ifndef THREADSLEEP_H
#define THREADSLEEP_H
#include "IThreadSleep.h"


class ThreadSleep : public IThreadSleep
{
public:
    ThreadSleep();
    virtual void SleepFor(const float seconds) override;

};

#endif // THREADSLEEP_H
