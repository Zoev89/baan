#ifndef ITHREADSLEEP_H
#define ITHREADSLEEP_H
#include "chrono"

class IThreadSleep
{
public:
     virtual void SleepFor(const float seconds) = 0;
protected:
    virtual ~IThreadSleep() {}

};

#endif // ITHREADSLEEP_H
