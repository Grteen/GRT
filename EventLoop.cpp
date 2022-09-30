#include "EventLoop.h"
#include "Log/Log.h"
#include "Epoller.h"
#include "Channel.h"
#include "Timer.h"
#include "Sockets.h"

#include <sys/epoll.h>
#include <poll.h>
#include <sys/eventfd.h>


// one loop per thread
// make sure every thread only has one loop

namespace grt
{

const int cEpollTimeOut = 10000;

__thread grt::EventLoop* oneLoopInThisThread = 0;

int createFd() {
    int fd = eventfd(0 , EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG(ERROR , "createFd failed");
        abort();
    }
    return fd;
} 

grt::EventLoop::EventLoop() 
    : is_loop_(false) ,
      threadId_(std::this_thread::get_id()) ,
      epoller_(new Epoller(this)) ,
      activeChannels_() ,
      is_quit_(false) ,
      timerQueue_(this) ,
      wakeFd_(createFd()) ,
      wakeChannel_(new Channel(this , wakeFd_)) ,
      is_CallingPendFunctors_(false)
{
    LOG(DEBUG , "EventLoop %x created in thread %ld" , this , threadId_);
    // if there are one loop in this thread
    if (oneLoopInThisThread) {
        LOG(ERROR , "Another EventLoop %x exists in this thread %ld" , &oneLoopInThisThread , threadId_);
    }
    else {
        oneLoopInThisThread = this;
    }

    wakeChannel_->setReadCallback(std::bind(&EventLoop::handleRead , this));
    wakeChannel_->enableReading();
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

        this->doPendFunctors();
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

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = sockets::read(this->wakeFd_ , &one , sizeof(one));
    if (n != sizeof(one)) {
        LOG(ERROR , "EventLoop::handleRead() reads %ld bytes instead of 8 bytes" , n);
    }
} 

int64_t EventLoop::runAfter(const TimerCallback& cb , int seconds , int milliseconds , int microseconds) {
    Timer timer(cb , seconds , milliseconds , microseconds);
    this->timerQueue_.addTimer(timer);
    return timer.timerId();
}

int64_t EventLoop::runEvery(const TimerCallback& cb , int interval_sec , int interval_mis , int interval_mcs) {
    Timer timer(cb ,  interval_sec , interval_mis , interval_mcs , true);
    this->timerQueue_.addTimer(timer);
    return timer.timerId();
}

void EventLoop::cancelTimer(int64_t timerId) {
    this->timerQueue_.cancelTimer(timerId);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = sockets::write(this->wakeFd_ , &one , sizeof(one));
    if (n != sizeof(one)) {
        LOG(ERROR , "EventLoop wakeup() writes %ld bytes intead of 8 bytes" , n);
    }
}

void EventLoop::doPendFunctors() {
    std::vector<Functor> pendingFunctors;
    this->is_CallingPendFunctors_ = true;

    {
        // use swap to decrease the length of critical zone
        std::lock_guard<std::mutex> locker(this->mutex_);
        pendingFunctors.swap(this->pendFunctors_);
    }

    for (auto func : pendingFunctors) {
        func();
    }
    
    this->is_CallingPendFunctors_ = false;
}

void EventLoop::queueInLoop(const Functor& cb) {
    {
        std::lock_guard<std::mutex> locker(this->mutex_);
        this->pendFunctors_.push_back(cb);
    }

    // if this Functor not in Loop's thread , wake EventLoop up
    // if EventLoop is calling pendFunctors_ , wake EventLoop up 
    // otherwise it will miss this new functors pushed back in pendFunctors_
    if (!this->isInLoopThread() || this->is_CallingPendFunctors_) {
        this->wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb) {
    if (this->isInLoopThread()) {
        // call function immediately
        cb();
    }
    else {
        this->queueInLoop(cb);
    }
}

}