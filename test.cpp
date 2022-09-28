#define GRT_DEBUG
#include "Buffer.h"
#include "Log/Log.h"
#include "EventLoop.h"
#include "Channel.h"
#include <iostream>
#include <sys/timerfd.h>
#include <unistd.h>
grt::EventLoop* g_loop;

void timeout() {
    printf("Time out\n");
    g_loop->quit();
}

int main() {
    grt::log::setLogLevelPermission(DEBUG , true);
    grt::EventLoop loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC , TFD_NONBLOCK | TFD_CLOEXEC);
    grt::Channel channel(&loop , timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong , sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd , 0 , &howlong , NULL);

    loop.loop();

    ::close(timerfd);
}