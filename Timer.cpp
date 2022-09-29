#include "Timer.h"

namespace grt
{

Timer::Timer(const TimerCallback& cb , int seconds , int milliseconds , int microseconds , bool repeat)
    : tmp_(std::chrono::system_clock::now()) , 
      repeat_(repeat) ,
      cb_(cb)
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

}

