#ifndef GRT_TCPCONNECTION_H
#define GRT_TCPCONNECTION_H

#include "../Base/Noncopyable.h"
#include "Callback.h"
#include "Buffer.h"
#include "Channel.h"
#include "InetAddr.h"
#include "Socket.h"

#include <memory>

namespace grt
{

class EventLoop;
class ThreadPool;

class TcpConnection : base::noncopyable ,
                      public std::enable_shared_from_this<TcpConnection> 
{
public:
    TcpConnection(EventLoop* loop , int sockfd , int connectionId , const InetAddr& localAddr , const InetAddr& peerAddr ,
                  std::shared_ptr<ThreadPool> threadPool);
    ~TcpConnection();

    void setReadFunction(const ReadFunction& rf) { this->readFunction_ = rf; }
    void setWriteFunction(const WriteFunction& wf) { this->writeFunction_ = wf; }
    void setComputFunction(const ComputFunction& cf) { this->computFunction_ = cf; }
    void setConnectionCallback(const ConnectionCallback& cc) { this->connectionCallback_ = cc; }
    void setCloseCallback(const CloseCallback& clc) { this->closeCallback_ = clc; }

    int connectionId() { return this->connectionId_; }
    int sockfd() { return this->socket_->sockfd(); }

    // enable channel's reading and call connectionCallback_;
    void connectEstablished();

    void connectDestroyed();

    // send message (message comes from outputBuffer_)
    void send();

    void shutdown();

    void setTcpNoDelay(bool on);

    EventLoop* getLoop() { return this->loop_; }

    Buffer* inputBuffer() { return &(this->inputBuffer_); }
    Buffer* computOverBuffer() { return &(this->computOverBuffer_); }
    Buffer* outputBuffer() { return &(this->outputBuffer_); }

private:
    enum State { cConnecting , cConnected , cDisconnected , cDisconnecting , };
    // when channel_ is active , it's readCallback will called this function
    void handleRead();
    // channel_'s closeCallback_ will call this function
    void handleClose();
    // write all outputBuffer_
    void handleWrite();

    void onMessageCallback();
    // called by onMessageCallback
    void readOverCallback();
    void computOverCallback();

    void setState(State state) { this->state_ = state; }

    // send all outputBuffer_
    void sendInLoop();

    void shutdownInLoop();

private:
    EventLoop* loop_;

    ReadFunction readFunction_;
    ComputFunction computFunction_;
    WriteFunction writeFunction_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;

    std::shared_ptr<ThreadPool> threadPool_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddr localAddr_;
    InetAddr peerAddr_;

    int connectionId_;
    State state_;

    Buffer inputBuffer_;
    Buffer computOverBuffer_;
    Buffer outputBuffer_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

}

#endif