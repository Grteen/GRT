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
#include "Acceptor.h"
#include "InetAddr.h"
#include "Sockets.h"
#include "BlockingQueue.h"
#include "TcpConnection.h"
using namespace grt;

void refun(Buffer& computBuf , Buffer& outputBuf) {
    std::cout << "read function call back" << std::endl;
}

void recom(Buffer& computBuf , Buffer& outputBuf) {
    std::cout << "recom function call back" << std::endl;
}

void wrcom(Buffer& outputBuf) {
    std::cout << "wrcom function call back" << std::endl;
}

int main(void) {
    EventLoop loop;
    grt::log::setLogLevelPermission(DEBUG , true);
    //grt::log::setLogLevelPermission(WARN , true);

    TcpConnection conn(&loop);

    conn.setReadFunction(&refun);
    conn.setComputFunction(&recom);
    conn.setWriteFunction(&wrcom);

    conn.onMessageCallback();

    loop.loop();

    while (1);
}