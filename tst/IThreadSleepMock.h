#ifndef ITHREADSLEEPMOCK_H
#define ITHREADSLEEPMOCK_H
#include "IThreadSleep.h"
#include "gmock/gmock.h"

class IThreadSleepMock: public IThreadSleep
{
public:
    MOCK_METHOD1(SleepFor, void(const float seconds));
};

#endif // ITHREADSLEEPMOCK_H
