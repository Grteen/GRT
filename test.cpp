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
#include "TcpServer.h"
using namespace grt;

void refun(Buffer& inputBuffer , Buffer& computBuf) {
    computBuf.append(inputBuffer.retrieveAllAsString());
}

void recom(Buffer& computBuf , Buffer& outputBuf) {
    outputBuf.append(computBuf.retrieveAllAsString());
}

void wrcom(Buffer& outputBuf) {

}

void onConnection(const TcpConnectionPtr& conn) {
    //std::cout << std::this_thread::get_id() << std::endl;
}

int count = 0;

void calcount() {
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << count << std::endl;
        count = 0;
    }
}

int main(void) {
    EventLoop loop;
    //grt::log::setLogLevelPermission(DEBUG , true);
    //grt::log::setLogLevelPermission(WARN , true);
    grt::log::setLogLevelPermission(INFO , false);
    //grt::log::setLogLevelPermission(ERROR , false);
    //grt::log::setLogLevelPermission(CRIT , false);
    InetAddr listening(9877);
    TcpServer server(&loop , listening);
    server.setConnectionCallback(onConnection);
    server.setReadFunction(refun);
    server.setComputFunction(recom);
    server.setWriteFunction(wrcom);
    server.setIOThreadNum(8);
    server.setComputThreadNum(0);
    std::thread t(calcount);
    server.start();
    loop.loop();
    t.join();

}