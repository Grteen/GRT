#include "EventLoop.h"
#include "Log/Log.h"
#include "Epoller.h"
#include "Channel.h"
#include "Timer.h"

#include <sys/epoll.h>
#include <poll.h>


// one loop per thread
// make sure every thread only has one loop

namespace grt
{

const int cEpollTimeOut = 10000;

__thread grt::EventLoop* oneLoopInThisThread = 0;

grt::EventLoop::EventLoop() 
    : is_loop_(false) ,
      threadId_(std::this_thread::get_id()) ,
      epoller_(new Epoller(this)) ,
      activeChannels_() ,
      is_quit_(false) ,
      timerQueue_(this)
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
    this->is_quit_ = false;

    while (!this->is_quit_) {
        this->activeChannels_.clear();
        this->epoller_->epoll(cEpollTimeOut , &this->activeChannels_);
        // handle all active channels' event
        for (auto x : this->activeChannels_) {
            x->handleEvent();
        }
    }

    LOG(DEBUG , "EventLoop %x stop looping" , this);
    this->is_loop_ = false;
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    this->assertInLoopThread();
    // let epoller to update this channel
    this->epoller_->updateChannel(channel);
}

void grt::EventLoop::quit() {
    this->is_quit_ = true;
    // wakeup
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

void EventLoop::runAfter(const TimerCallback& cb , int seconds , int milliseconds , int microseconds) {
    Timer timer(cb , seconds , milliseconds , microseconds);
    this->timerQueue_.addTimer(timer);
}

}