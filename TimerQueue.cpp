#include "TimerQueue.h"
#include "Log/Log.h"
#include "Timer.h"
#include "EventLoop.h"

#include <unistd.h>

namespace grt
{

// create a timerfd , used by TimerQueue's ctor
int createTimerfd() {
    int timerfd = timerfd_create(CLOCK_MONOTONIC , TFD_CLOEXEC | TFD_NONBLOCK);

    if (timerfd < 0) {
        LOG(CRIT , "Failed to create timerfd");
    }
    return timerfd;
}

// allow time_point convert to timespec
constexpr timespec timepointToTimespec(std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp ){
    auto secs = std::chrono::time_point_cast<std::chrono::seconds>(tp);
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp) -
             std::chrono::time_point_cast<std::chrono::nanoseconds>(secs);

    return timespec{secs.time_since_epoch().count(), ns.count()};
}

// allow duration convert to timespec
constexpr timespec durationToTimespec(std::chrono::nanoseconds dur)
{
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(dur);
    dur -= secs;

    return timespec{secs.count(), dur.count()};
}

// read timerfd
void readTimerfd(int timerfd) {
    uint64_t expiredNum = 0;
    ssize_t n = read(timerfd , &expiredNum , sizeof(expiredNum));
    LOG(DEBUG , "TimerQueue::handleRead() %ld" , expiredNum);
    if (n != sizeof(expiredNum)) {
        LOG(ERROR , "TimerQueue::handleRead() reads %ld bytes instead of 8 bytes" , n);
    }
}

std::chrono::microseconds delayTime(Timer& timer) {
    auto delaytime = timer.time_point().time_since_epoch() - std::chrono::system_clock::now().time_since_epoch();
    // if time difference is less than 200 microseconds , return 200 microseconds
    if (delaytime.count() < std::chrono::microseconds(200).count()) {
        auto delay = std::chrono::microseconds(200);
        return delay;
    }
    // if not , return 0 microseconds means set the timerfd's wake time is this timer's time
    auto delay = std::chrono::microseconds(0);
    return delay;
}

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop) , 
      timerfd_(createTimerfd()) , 
      timerfdChannel_(loop , timerfd_) ,
      timers_() 
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead , this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue() {
    close(this->timerfd_);
}

bool TimerQueue::insertTimer(Timer& timer) {
    this->loop_->assertInLoopThread();
    bool eariest = false;
    // this timer will expire firstly
    if (this->timers_.empty() || timer < *(this->timers_.begin())) {
        eariest = true;
    }

    this->timers_.insert(std::move(timer));
    return eariest;
}

void TimerQueue::resetTimerfd() {
    auto firstTimerIt = this->timers_.begin();
    Timer firstTimer = *firstTimerIt;
    struct itimerspec newWakeTime;
    memset(&newWakeTime , 0 , sizeof(newWakeTime));
    auto delaytime = delayTime(firstTimer);
    auto newTime = std::chrono::nanoseconds(0);

    if (delaytime != std::chrono::microseconds(0)) {
        newTime = delaytime;
    }
    else {
        newTime = firstTimer.time_point() - std::chrono::system_clock::now();
    }

    newWakeTime.it_value = durationToTimespec(newTime);
    // reset timerfd's wake up time
    int ret = timerfd_settime(this->timerfd_, 0 , &newWakeTime, NULL);
    if (ret) {
        LOG(ERROR , "timerfd_settime");
    }
    LOG(DEBUG , "Timerfd reset after %ld" , newTime.count());
}

void TimerQueue::addTimer(Timer& timer) {
    this->loop_->assertInLoopThread();
    bool eariestChanged = this->insertTimer(timer);

    LOG(INFO , "new Timer set");
    if (eariestChanged == true) {
        // the first timer to wake up is changed , so the timerfd_'s wake time will also change 
        this->resetTimerfd();
    }
}

std::vector<Timer> TimerQueue::getExpired() {
    std::vector<Timer> expired;
    Timer nowTime(std::chrono::system_clock::now());
    // find all timer whose tmp_ is smaller than nowTime
    auto it = this->timers_.lower_bound(nowTime);
    std::move(this->timers_.begin() , it , std::back_inserter(expired));
    this->timers_.erase(this->timers_.begin() , it);

    return expired;
}

void TimerQueue::resetExpired(const std::vector<Timer>& expired) {
    for (auto timer : expired) {
        if (timer.repeat()) {
            this->addTimer(timer);
        }
    }
}

void TimerQueue::handleRead() {
    this->loop_->assertInLoopThread();
    readTimerfd(this->timerfd_);

    std::vector<Timer> expired = this->getExpired();

    for (auto timer : expired) {
        timer.runCallbackFunction();
    }

    this->resetExpired(expired);
}


}
