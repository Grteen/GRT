#include "TcpConnection.h"
#include "ThreadPool.h"
#include "EventLoop.h"
#include "Socket.h"
#include "../Log/Log.h"

#include <unistd.h>
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
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose , this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite , this));
}

TcpConnection::~TcpConnection() {
    LOG(DEBUG , "TcpConnection died sockfd = %ld" , this->socket_->sockfd());
}

void TcpConnection::onMessageCallback() {
    this->loop_->assertInLoopThread();
    ssize_t n = this->inputBuffer_.readFd(this->socket_->sockfd() , NULL);
    if (n > 0) {
        // message call back
        if (this->readFunction_) {
            this->readFunction_(shared_from_this());
            if (this->computFunction_) {
                if (!this->threadPool_->isEmpty()) {
                    // computThreadPool can work
                    this->threadPool_->puttask(std::bind(&TcpConnection::readOverCallback , shared_from_this()));
                }
                else {
                    // no computThreadPool
                    this->readOverCallback();
                }
            }
            else if (this->writeFunction_) {
                // no computFunction but has writeFunction , so call wirteFunction immediately
                this->writeFunction_(shared_from_this());
            }
        }
    }
    else if (n == 0) {
        // close call back
        this->handleClose();
    }
    else {
        // handle error
        LOG(ERROR , "onMessageCallback error");
    }
}

void TcpConnection::readOverCallback() {
    this->computFunction_(shared_from_this());
    this->loop_->runInLoop(std::bind(&TcpConnection::computOverCallback , shared_from_this()));
}  

void TcpConnection::computOverCallback() {
    if (this->writeFunction_) {
        this->writeFunction_(shared_from_this());
    }
    // if outputBuf have some messages
    // send the outputBuf

    // if (this->outputBuffer_.readableBytes()) {
    //     this->send();
    // }
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

void TcpConnection::handleClose() {
    this->loop_->assertInLoopThread();
    LOG(DEBUG , "TcpConnection::handleClose [fd = %ld]" , this->socket_->sockfd());
    assert(this->state_ == cConnected || this->state_ == cDisconnecting);
    this->channel_->disableAll();
    this->closeCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    this->loop_->assertInLoopThread();
    assert(this->state_ == cConnected);
    this->setState(cDisconnected);
    this->channel_->disableAll();
    // connectionCloseCallback
    this->loop_->removeChannel(this->channel_.get());
}

void TcpConnection::send() {
    if (this->state_ == cConnected) {
        if (this->loop_->isInLoopThread()) {
            this->sendInLoop();
        }
        else {
            this->loop_->runInLoop(std::bind(&TcpConnection::sendInLoop , this));
        }
    }
}

void TcpConnection::sendInLoop() {
    this->loop_->assertInLoopThread();
    if (!this->channel_->isWriting()) {
        this->channel_->enableWriting();
    }
}

void TcpConnection::shutdown() {
    if (this->state_ == cConnected) {
        // state set to cDisconnecting so the Write end can be closed
        this->setState(cDisconnecting);
        this->loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop , this));
    }
}

void TcpConnection::shutdownInLoop() {
    this->loop_->assertInLoopThread();
    if (!this->channel_->isWriting()) {
        // if write is over close the wrting end
        this->socket_->shutdownWrite();
    }
}

void TcpConnection::handleWrite() {
    this->loop_->assertInLoopThread();
    if (this->channel_->isWriting()) {
        ssize_t n = ::write(this->channel_->fd() , this->outputBuffer_.peek().c_str() , this->outputBuffer_.readableBytes());
        // if (n == 0) {
        //     std::cout << outputBuffer_.peek() << std::endl;
        //     std::cout << outputBuffer_.readableBytes() << std::endl;
        // }
        if (n > 0) {
            LOG(DEBUG , "Write message %ld bytes Success [fd = %ld] " , n , this->channel_->fd());
            this->outputBuffer_.retrieve(n);
            if (this->outputBuffer_.readableBytes() == 0) {
                // write is over
                this->channel_->disableWriting();
                if (this->state_ == cDisconnecting) {
                    this->shutdownInLoop();
                }
            }
            else {
                LOG(DEBUG , "Write is not over , rest: %ld" , this->outputBuffer_.readableBytes());
            }
        }
        else {
            LOG(ERROR , "TcpConnection::handleWrite");
            this->channel_->disableWriting();
        }
    }
    else {
        LOG(DEBUG , "Connection is down , no more writing");
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    this->socket_->setTcpNoDelay(on);
}

}