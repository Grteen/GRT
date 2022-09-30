#define GRT_DEBUG
#include "Buffer.h"
#include "Log/Log.h"
#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThread.h"
#include <iostream>
#include <sys/timerfd.h>
#include <unistd.h>
#include <chrono>
grt::EventLoop* g_loop;
int64_t id;

void timeout() {
    std::cout << "timeout" << std::endl;
}

void thr() {
    id = g_loop->runAfter(timeout , 5);
    g_loop->cancelTimer(id);
}

int main() {
    auto it = std::this_thread::get_id();
    std::cout << it << std::endl;
    grt::log::setLogLevelPermission(DEBUG , true);
    grt::EventLoopThread loop;
    grt::EventLoop* l = loop.startLoop();
    l->runAfter(timeout , 5);
}