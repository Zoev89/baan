#ifndef DISPATCHER_H
#define DISPATCHER_H
#include <future>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <queue>

class Dispatcher
{
public:
    Dispatcher();
    ~Dispatcher();
    void Dispatch(std::function<void(void)> func);

private:
    void Worker();
    bool mStop;
    std::future<void> WorkerFuture;
    std::queue<std::function<void(void)>> workQueue;
    std::mutex dataMutex;
    std::condition_variable execWorker;
};

#endif // DISPATCHER_H
