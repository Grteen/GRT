#include "Timeoperator.h"

#include <chrono>
#include <string.h>

using namespace grt;
using namespace base;

// return the nowTime in char* 
// Tue Sep 27 22:16:32 2022
const char* grt::base::nowTime() {
    auto nowtime_clock = std::chrono::system_clock::now();
    auto nowtime = std::chrono::system_clock::to_time_t(nowtime_clock);
    auto res = ctime(&nowtime);
    res[strlen(res) - 1] = 0;
    return res;
}