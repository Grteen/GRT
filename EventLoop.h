#ifndef GRT_EVENTLOOP_H
#define GRT_EVENTLOOP_H

#include <thread>

#include "./Base/Noncopyable.h"

namespace grt
{

class EventLoop : base::noncopyable {
public:
    EventLoop();
    ~EventLoop();

    void loop();

    // used to assert , make sure in this loop's thread
    void assertInLoopThread();

    bool isInLoopThread() const { return this->threadId_ == std::this_thread::get_id(); }
private:

    void abortNotInLoopThread();

    bool is_loop_;      
    // this loop's thread id
    const std::thread::id threadId_;
};
}


#endif