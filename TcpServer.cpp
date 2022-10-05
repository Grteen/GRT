#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Log/Log.h"
#include "Sockets.h"
#include "ThreadPool.h"
#include "EventLoopThreadPool.h"

#include <iostream>

using namespace std::placeholders;

namespace grt
{

ThreadPool g_computPool(0);

TcpServer::TcpServer(EventLoop* loop , const InetAddr& listenAddr) 
    : loop_(loop) ,
      acceptor_(new Acceptor(loop , listenAddr)) , 
      started_(false) ,
      currentConnId_(0) ,
      connections_() ,
      threadPool_(&g_computPool) ,
      loopPool_(new EventLoopThreadPool(loop))
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::handleConnection , this , _1 , _2));
}

TcpServer::~TcpServer() {

}

void TcpServer::handleConnection(int sockfd , const InetAddr& peerAddr) {
    this->loop_->assertInLoopThread();
    int newConnectionId = this->currentConnId_;
    currentConnId_++;

    LOG(INFO , "TcpServer::handleConnection - new connection [id = %ld][fd = %ld] from %s" , newConnectionId , 
        sockfd , peerAddr.toHostPort().c_str());
    InetAddr localAddr(sockets::getLocalAddr(sockfd));
    // find the ioLoop in loopPool_
    EventLoop* ioLoop = this->loopPool_->getNextLoop();
    // create a new connecton
    TcpConnectionPtr conn(
        new TcpConnection(ioLoop , sockfd , newConnectionId , localAddr , peerAddr , this->threadPool_));
    
    this->connections_.insert(std::make_pair(newConnectionId , conn));
    conn->setConnectionCallback(this->connectionCallback_);
    conn->setReadFunction(this->readFunction_);
    conn->setComputFunction(this->computFunction_);
    conn->setWriteFunction(this->writeFunction_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection , this , _1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished , conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
    this->loop_->assertInLoopThread();
    LOG(INFO , "TcpServer::removeConnection [fd = %ld]" , conn->sockfd());
    size_t n = this->connections_.erase(conn->connectionId());
    assert(n == 1);
    // connectDestroyed must be in connection's ownerLoop
    EventLoop* ioLoop = conn->getLoop();
    this->loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed , conn));
}

void TcpServer::setIOThreadNum(int threadnum) {
    this->loopPool_->setThreadNum(threadnum);
}

void TcpServer::setComputThreadNum(int threadnum) {
    this->threadPool_->setThreadNum(threadnum);
}

void TcpServer::start() {
    if (!this->started_) {
        this->started_ = true;
        this->threadPool_->start();
        this->loopPool_->start();
    }

    if (!this->acceptor_->listening()) {
        this->loop_->runInLoop(std::bind(&Acceptor::listen , this->acceptor_.get()));
    }
}

}