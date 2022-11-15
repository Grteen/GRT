#ifndef GRT_TIMER_H
#define GRT_TIMER_H

#include "Callback.h"

#include <chrono>
#include <atomic>

namespace grt
{
class Timer {
public:
    Timer() = delete;
    Timer(const TimerCallback& cb , int seconds , int milliseconds = 0, int microseconds = 0 , bool repeat = false);
    Timer(std::chrono::time_point<std::chrono::system_clock> time_point);

    ~Timer();

    bool operator< (const Timer& rhs) const {
        return this->tmp_.time_since_epoch().count() < rhs.tmp_.time_since_epoch().count();
    }

    bool repeat() { return this->repeat_; }

    int64_t timerId() { return this->timerId_; }

    std::chrono::time_point<std::chrono::system_clock> time_point() { return this->tmp_; }

    void runCallbackFunction() { this->cb_(); }

    void setRepeat(bool on) { this->repeat_ = on; }

    // reset the timer and it will be added into timerqueue again
    void timerRepeatReset();

private:   
    // time since epoch
    std::chrono::time_point<std::chrono::system_clock> tmp_;
    bool repeat_;
    TimerCallback cb_;

    std::chrono::seconds second_;
    std::chrono::milliseconds millsecond_;
    std::chrono::microseconds micrsecond_;

    int64_t timerId_;

    static std::atomic_int64_t timerIdCreater;
};

}

#endif