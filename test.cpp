#define GRT_DEBUG
#include "Buffer.h"
#include "Log/Log.h"
#include "EventLoop.h"
#include "Channel.h"
#include <iostream>
#include <sys/timerfd.h>
#include <unistd.h>
#include <chrono>
grt::EventLoop* g_loop;

void timeout() {
    std::cout << "timeout" << std::endl;
}

int main() {
    grt::log::setLogLevelPermission(DEBUG , true);
    grt::EventLoop loop;
    g_loop = &loop;

    g_loop->runAfter(timeout , 5);
    std::cout << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;

    loop.loop();
}