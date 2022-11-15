#include "EventLoopThread.h"
#include "EventLoop.h"
#include <assert.h>

namespace grt
{

EventLoopThread::EventLoopThread()
    : loop_(NULL) , 
      thread_() ,
      mutex_() ,
      cond_()
{

}

EventLoopThread::~EventLoopThread() {
    this->loop_->quit();
    this->thread_.join();
}

EventLoop* EventLoopThread::startLoop() {
    this->thread_ = std::thread(std::bind(&EventLoopThread::threadFunc , this));

    {
        std::unique_lock<std::mutex> locker(this->mutex_);
        while (this->loop_ == NULL) {
            this->cond_.wait(locker);
        }
    }

    return this->loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    {
        std::lock_guard locker(this->mutex_);
        this->loop_ = &loop;
        this->cond_.notify_one();
    }

    this->loop_->loop();
}

}