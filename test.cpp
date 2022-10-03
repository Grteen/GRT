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
    std::string test = "how are you!";
    computBuf.append(test);
}

void recom(Buffer& computBuf , Buffer& outputBuf) {
    std::string res = computBuf.retrieveAllAsString();
    outputBuf.append("6666");
}

void wrcom(Buffer& outputBuf) {
    std::string res = outputBuf.retrieveAllAsString();
    std::cout << res << std::endl;
}

void onConnection(const TcpConnectionPtr& conn) {
    std::cout << "tcpconnection here" << std::endl;
}

int main(void) {
    EventLoop loop;
    //grt::log::setLogLevelPermission(DEBUG , true);
    //grt::log::setLogLevelPermission(WARN , true);
    InetAddr listening(9877);
    TcpServer server(&loop , listening);
    server.setConnectionCallback(onConnection);
    server.setReadFunction(refun);
    server.setComputFunction(recom);
    server.setWriteFunction(wrcom);
    server.start();
    loop.loop();
}