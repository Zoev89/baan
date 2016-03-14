#include "Dispatcher.h"
#include <iostream>

Dispatcher::Dispatcher()
    : mStop(false)
{
    WorkerFuture = std::async(std::launch::async, [this](){ Worker(); });
}

Dispatcher::~Dispatcher()
{
    mStop = true;
    execWorker.notify_one();
    WorkerFuture.wait();
}

void Dispatcher::Worker()
{
    while (mStop == false)
    {
        std::function<void(void)> func;
        {
            std::unique_lock<std::mutex> lock(dataMutex);
            execWorker.wait(lock, [&]{return ((mStop == true) || (!workQueue.empty()));});
            if (mStop==false)
            {
                func = workQueue.front();
                workQueue.pop();
            }
        }
        if (func)
        {
            func();
        }
    }
}

void Dispatcher::Dispatch(std::function<void(void)> func)
{
    std::lock_guard<std::mutex> lock(dataMutex);
    workQueue.push(func);
    execWorker.notify_one();
}
