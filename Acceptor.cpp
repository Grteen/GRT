#include "Acceptor.h"
#include "Sockets.h"
#include "EventLoop.h"
#include "InetAddr.h"

namespace grt
{

Acceptor::Acceptor(EventLoop* loop , const InetAddr& addr)
    : loop_(loop) ,
      acceptsockfd_(sockets::createNonblockSocket(AF_INET)) ,
      acceptChannel_(loop , acceptsockfd_.sockfd()) ,
      listening_(false)
{
    acceptsockfd_.bindAddress(addr);
    // when acceptsockfd can be read , call the handleRead
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead , this));
}

void Acceptor::listen() {
    this->loop_->assertInLoopThread();
    this->listening_ = true;
    this->acceptsockfd_.listen();
    this->acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
    this->loop_->assertInLoopThread();
    InetAddr peerAddr(0);
    int connfd = this->acceptsockfd_.accept(&peerAddr);
    if (connfd >= 0) {
        if (this->connectionCallback_) {
            // call back the function
            this->connectionCallback_(connfd , peerAddr);
        }
        else {
            sockets::close(connfd);
        }
    }
}

}