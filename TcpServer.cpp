#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Log/Log.h"
#include "Sockets.h"
#include "ThreadPool.h"
#include <iostream>

using namespace std::placeholders;

namespace grt
{

ThreadPool g_computPool(4);

TcpServer::TcpServer(EventLoop* loop , const InetAddr& listenAddr) 
    : loop_(loop) ,
      acceptor_(new Acceptor(loop , listenAddr)) , 
      started_(false) ,
      currentConnId_(0) ,
      connections_() ,
      threadPool_(&g_computPool)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::handleConnection , this , _1 , _2));
}

TcpServer::~TcpServer() {

}

void TcpServer::handleConnection(int sockfd , const InetAddr& peerAddr) {
    this->loop_->assertInLoopThread();
    int newConnectionId = this->currentConnId_;
    currentConnId_++;

    LOG(INFO , "TcpServer::handleConnection - new connection [id = %ld] from %s" , newConnectionId , peerAddr.toHostPort().c_str());
    InetAddr localAddr(sockets::getLocalAddr(sockfd));

    // create a new connecton
    TcpConnectionPtr conn(
        new TcpConnection(this->loop_ , newConnectionId , sockfd , localAddr , peerAddr , this->threadPool_));
    
    this->connections_.insert(std::make_pair(newConnectionId , conn));
    conn->setConnectionCallback(this->connectionCallback_);
    conn->setReadFunction(this->readFunction_);
    conn->setComputFunction(this->computFunction_);
    conn->setWriteFunction(this->writeFunction_);
    conn->connectEstablished();
}

void TcpServer::start() {
    if (!this->started_) {
        this->started_ = true;
        this->threadPool_->start();
    }

    if (!this->acceptor_->listening()) {
        this->loop_->runInLoop(std::bind(&Acceptor::listen , this->acceptor_.get()));
    }
}

}