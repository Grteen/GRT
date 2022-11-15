#ifndef GRT_EVENTLOOPTHREAD_H
#define GRT_EVENTLOOPTHREAD_H

#include "../Base/Noncopyable.h"
#include "../Base/Mutexoperator.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace grt
{

class EventLoop;

// start loop in other threads
class EventLoopThread : base::noncopyable {
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* startLoop();
private:
    void threadFunc();

private:
    EventLoop* loop_;
    std::mutex mutex_;
    std::condition_variable cond_ GUARDED_BY(this->mutex_);
    std::thread thread_;
};

}

#endif