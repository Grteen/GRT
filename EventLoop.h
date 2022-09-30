#ifndef GRT_EVENTLOOP_H
#define GRT_EVENTLOOP_H

#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

#include "./Base/Noncopyable.h"
#include "Callback.h"
#include "TimerQueue.h"
#include "./Base/Mutexoperator.h"

namespace grt
{
class Channel;
class Epoller;

class EventLoop : base::noncopyable {
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    // used to assert , make sure in this loop's thread
    void assertInLoopThread();

    bool isInLoopThread() const { return this->threadId_ == std::this_thread::get_id(); }

    void updateChannel(Channel* channel);

    int64_t runAfter(const TimerCallback& cb , int seconds , int milliseconds = 0, int microseconds = 0);

    int64_t runEvery(const TimerCallback& cb , int interval_sec , int interval_mis = 0 , int interval_mcs = 0);

    void cancelTimer(int64_t timerId);

    // wake the EventLoop
    void wakeup();

    // put the Functor into pendingFunctors_
    // used to make sure Functor will be called in EventLoop's thread
    void queueInLoop(const Functor& cb); 

    // safe to call from other threads
    // if in loop's thread , call this function immediately
    // otherwise call queueInLoop
    void runInLoop(const Functor& cb);

private:
    void abortNotInLoopThread();
    // used to read wakeFd_;
    void handleRead(); 

    // deal with all pendFunctors , this function will be called after epoll
    void doPendFunctors();

private:
    typedef std::vector<Channel*> ChannelList;

    bool is_loop_;      
    bool is_quit_;
    // this loop's thread id
    const std::thread::id threadId_;

    std::unique_ptr<Epoller> epoller_;
    ChannelList activeChannels_;

    TimerQueue timerQueue_;

    // all pending functor will be pushed into this vector at first
    // after epoller finished , EventLoop will deal with this vector
    std::vector<Functor> pendFunctors_ GUARDED_BY(this->mutex_);
    std::mutex mutex_;
    bool is_CallingPendFunctors_;

    // used to wake up the EventLoop when it blocked on epoller_
    int wakeFd_;
    std::unique_ptr<Channel> wakeChannel_;
};
}


#endif