#include "ThreadSleep.h"
#include <thread>

ThreadSleep::ThreadSleep()
{
}


void  ThreadSleep::SleepFor(const float seconds)
{
    std::chrono::duration<float> duration(seconds);

    std::this_thread::sleep_for(duration);
}

