#define GRT_DEBUG
#include "Buffer.h"
#include "Log/Log.h"
#include "EventLoop.h"
#include <iostream>

grt::EventLoop* g_loop;

void threadfunc() {
    g_loop->loop();
}
int main(void) {
    grt::log::setLogLevelPermission(grt::log::cDebug , true);
    grt::EventLoop loop;
    g_loop = &loop;

    std::thread t(threadfunc);
    t.join();
    while(1);
}