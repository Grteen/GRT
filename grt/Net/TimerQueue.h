#ifndef GRT_TIMERQUEUE_H
#define GRT_TIMERQUEUE_H

#include "../Base/Noncopyable.h"
#include "Channel.h"

#include <sys/timerfd.h>
#include <set>
#include <vector>

namespace grt
{

class Timer;
class EventLoop;

class TimerQueue : base::noncopyable {
public:
    TimerQueue() = delete;
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    void addTimer(Timer& timer);

    void cancelTimer(int64_t timerId);

private:
    // when timerfd_ returned by epoller , call timerfdChannel_'s handleEvent
    // which is this function
    void handleRead();

    // insert Timer into timers_ 
    // if this timer is the first timer to expire return true , else return false
    bool insertTimer(Timer& timer);

    void insertCancelTimer(int64_t timerId);

    // reset the timerfd_'s wake time
    void resetTimerfd();

    // get all expired timer
    std::vector<Timer> getExpired();

    // check all expired timer , if timer is repeated and add it again
    void resetExpired(const std::vector<Timer>& expired);

    void addTimerInLoop(Timer& timer);

    void cancelTimerInLoop(int64_t timerId);

private:
    // timer's file descriptor which used by EventLoop's epoller
    const int timerfd_;
    Channel timerfdChannel_;
    std::set<Timer> timers_;
    // all canceled Timers' timerid is stored in here
    std::set<int64_t> cancelTimers_;

    EventLoop* loop_;
};

}

#endif