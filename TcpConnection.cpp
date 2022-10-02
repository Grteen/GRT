#include "TcpConnection.h"
#include "ThreadPool.h"
#include "EventLoop.h"

namespace grt
{

TcpConnection::TcpConnection(EventLoop* loop) 
    : loop_(loop) ,
      threadPool_(new ThreadPool(4))
{
    this->threadPool_->start();
}

TcpConnection::~TcpConnection() {

}

void TcpConnection::onMessageCallback() {
    this->loop_->assertInLoopThread();
    if (this->readFunction_) {
        this->readFunction_(this->inputBuffer_ , this->computOverBuffer_);
        if (this->computFunction_) {
            this->threadPool_->puttask(std::bind(&TcpConnection::readOverCallback , this , 
                                               this->inputBuffer_ , this->computOverBuffer_));
        }
        else if (this->writeFunction_) {
            this->writeFunction_(this->outputBuffer_);
        }
    }
}

void TcpConnection::readOverCallback(Buffer& computBuf , Buffer& outputBuf) {
    this->computFunction_(computBuf , outputBuf);
    if (this->writeFunction_) {
        this->loop_->runInLoop(std::bind(&TcpConnection::computOverCallback , this , this->outputBuffer_));
    }
}  

void TcpConnection::computOverCallback(Buffer& outputBuf) {
    this->writeFunction_(outputBuf);
}

}