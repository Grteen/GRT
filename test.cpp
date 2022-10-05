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
    std::cout << "read function called" << std::endl;
}

void recom(Buffer& computBuf , Buffer& outputBuf) {
    std::string res = computBuf.retrieveAllAsString();
    outputBuf.append("6666");
}

void wrcom(Buffer& outputBuf) {

}

void onConnection(const TcpConnectionPtr& conn) {
    std::cout << std::this_thread::get_id() << std::endl;
}

int main(void) {
    EventLoop loop;
    grt::log::setLogLevelPermission(DEBUG , true);
    //grt::log::setLogLevelPermission(WARN , true);
    InetAddr listening(9877);
    TcpServer server(&loop , listening);
    server.setConnectionCallback(onConnection);
    server.setReadFunction(refun);
    server.setComputFunction(recom);
    server.setWriteFunction(wrcom);
    server.setIOThreadNum(2);
    server.setComputThreadNum(2);
    server.start();
    loop.loop();
}