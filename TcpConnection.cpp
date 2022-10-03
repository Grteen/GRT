#include "TcpConnection.h"
#include "ThreadPool.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Log/Log.h"
#include <iostream>

namespace grt
{

TcpConnection::TcpConnection(EventLoop* loop , int sockfd , int connectionId , const InetAddr& localAddr , const InetAddr& peerAddr ,
                              std::shared_ptr<ThreadPool> threadPool)
    : loop_(loop) , 
      socket_(new Socket(sockfd)) ,
      channel_(new Channel(loop , sockfd)) ,
      localAddr_(localAddr) ,
      peerAddr_(peerAddr_) ,
      connectionId_(connectionId) , 
      state_(cConnecting) ,
      threadPool_(threadPool)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead , this));
}

TcpConnection::~TcpConnection() {
    LOG(DEBUG , "TcpConnection died sockfd = %ld" , this->socket_->sockfd());
}

void TcpConnection::onMessageCallback() {
    this->loop_->assertInLoopThread();
    if (this->readFunction_) {
        this->readFunction_(this->inputBuffer_ , this->computOverBuffer_);
        if (this->computFunction_) {
            this->threadPool_->puttask(std::bind(&TcpConnection::readOverCallback , shared_from_this() , 
                                                 std::ref(this->computOverBuffer_) , std::ref(this->outputBuffer_)));
        }
        else if (this->writeFunction_) {
            this->writeFunction_(this->outputBuffer_);
        }
    }
}

void TcpConnection::readOverCallback(Buffer& computBuf , Buffer& outputBuf) {
    this->computFunction_(computBuf , outputBuf);
    if (this->writeFunction_) {
        this->loop_->runInLoop(std::bind(&TcpConnection::computOverCallback , shared_from_this() , std::ref(this->outputBuffer_)));
    }
}  

void TcpConnection::computOverCallback(Buffer& outputBuf) {
    this->writeFunction_(outputBuf);
}

void TcpConnection::handleRead() {
    this->onMessageCallback();
}

void TcpConnection::connectEstablished() {
    this->loop_->assertInLoopThread();
    assert(this->state_ == cConnecting);
    this->setState(cConnected);
    this->channel_->enableReading();
    this->connectionCallback_(shared_from_this());
}

}