#include "Timer.h"
#include <iostream>

namespace grt
{

std::atomic_int64_t Timer::timerIdCreater(0);

Timer::Timer(const TimerCallback& cb , int seconds , int milliseconds , int microseconds , bool repeat)
    : tmp_(std::chrono::system_clock::now()) , 
      repeat_(repeat) ,
      cb_(cb) ,
      second_(seconds) ,
      millsecond_(milliseconds) ,
      micrsecond_(microseconds) ,
      timerId_(timerIdCreater.fetch_add(1))
{   
    // set time_point's value (means after how long it will expired)
    tmp_ += std::chrono::seconds(seconds);
    tmp_ += std::chrono::milliseconds(milliseconds);
    tmp_ += std::chrono::microseconds(microseconds);
}

Timer::Timer(std::chrono::time_point<std::chrono::system_clock> time_point) 
    : tmp_(time_point)
{

}

Timer::~Timer() {}

void Timer::timerRepeatReset() {
    this->tmp_ = std::chrono::system_clock::now();
    tmp_ += std::chrono::seconds(this->second_);
    tmp_ += std::chrono::milliseconds(this->millsecond_);
    tmp_ += std::chrono::microseconds(this->micrsecond_);
}

}



