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
grt::EventLoop* g_loop;
int64_t id;

void Connection(int sockfd , const grt::InetAddr& peerAddr) {
    printf("%s\n" , peerAddr.toHostPort().c_str());
    write(sockfd , "fuck you!\n" , 9);
    grt::sockets::close(sockfd);
}

int main(void) {
    grt::InetAddr listenAddr(9877);
    grt::EventLoop loop;
    grt::Acceptor acceptor(&loop , listenAddr);
    acceptor.setNewConnectionCallback(Connection);
    acceptor.listen();
    loop.loop();
}