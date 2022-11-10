#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include <assert.h>

namespace grt
{

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)  
    : baseLoop_(baseLoop) ,
    started_(false) ,
    threadnum_(0) ,
    nextLoop_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool() {

}

void EventLoopThreadPool::start() {
    assert(!this->started_);
    this->baseLoop_->assertInLoopThread();
    this->started_ = true;

    // create EventLoop
    for (int i = 0 ; i < this->threadnum_ ; i++) {
        EventLoopThreadPtr ptr(new EventLoopThread);
        this->threads_.push_back(ptr);
        // EventLoopThread start will return it's EventLoop
        this->loops_.push_back(ptr->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    this->baseLoop_->assertInLoopThread();
    EventLoop* loop = this->baseLoop_;

    if (!this->loops_.empty()) {
        // round - robin
        loop = this->loops_[this->nextLoop_];
        this->nextLoop_++;
        if (static_cast<size_t>(this->nextLoop_) >= this->loops_.size()) {
            this->nextLoop_ = 0;
        }
    }
    return loop;
}

}