#ifndef GRT_EVENTLOOPTHREADPOOL_H
#define GRT_EVENTLOOPTHREADPOOL_H

#include "../Base/Noncopyable.h"

#include <vector>
#include <memory>

namespace grt
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : base::noncopyable {
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void setThreadNum(int threadnum) { this->threadnum_ = threadnum; }
    void start();

    EventLoop* getNextLoop();
private:

private:
    typedef std::shared_ptr<EventLoopThread> EventLoopThreadPtr;
    EventLoop* baseLoop_;
    int threadnum_;
    int nextLoop_;
    bool started_;
    std::vector<EventLoopThreadPtr> threads_;
    std::vector<EventLoop*> loops_;
};

}

#endif