#include "EventLoop.h"
#include "Log/Log.h"

#include <sys/epoll.h>
#include <poll.h>


// one loop per thread
// make sure every thread only has one loop

namespace grt
{

__thread grt::EventLoop* oneLoopInThisThread = 0;

grt::EventLoop::EventLoop() 
    : is_loop_(false) ,
      threadId_(std::this_thread::get_id())
{
    LOG(DEBUG , "EventLoop %x created in thread %ld" , this , threadId_);
    // if there are one loop in this thread
    if (oneLoopInThisThread) {
        LOG(ERROR , "Another EventLoop %x exists in this thread %ld" , &oneLoopInThisThread , threadId_);
    }
    else {
        oneLoopInThisThread = this;
    }
}


grt::EventLoop::~EventLoop() {
    assert(!this->is_loop_);
    oneLoopInThisThread = 0;
}

void grt::EventLoop::loop() {
    assert(!this->is_loop_);
    this->assertInLoopThread();

    this->is_loop_ = true;
    ::poll(NULL , 0 , 5 * 1000);

    LOG(DEBUG , "EventLoop %x stop looping" , this);
    this->is_loop_ = false;
}

void grt::EventLoop::assertInLoopThread() {
    // if not in loop's thread , call abort
    if (!this->isInLoopThread()) {
        this->abortNotInLoopThread();
    }
}

void grt::EventLoop::abortNotInLoopThread() {
    LOG(CRIT , "EventLoop::abortNotInLoopThread , EventLoop was created in threadId %ld " 
               "current threadId is %ld" , this->threadId_ , std::this_thread::get_id());
}

}