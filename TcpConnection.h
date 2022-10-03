#ifndef GRT_TCPCONNECTION_H
#define GRT_TCPCONNECTION_H

#include "./Base/Noncopyable.h"
#include "Callback.h"
#include "Buffer.h"
#include "Channel.h"
#include "InetAddr.h"

#include <memory>

namespace grt
{

class EventLoop;
class ThreadPool;
class Socket;

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

    // enable channel's reading and call connectionCallback_;
    void connectEstablished();

private:
    enum State { cConnecting , cConnected , };
    // when channel_ is active , it's readCallback will called this function
    void handleRead();

    void onMessageCallback();
    // called by onMessageCallback
    void readOverCallback(Buffer& computBuf , Buffer& outputBuf);
    void computOverCallback(Buffer& outputBuf);

    void setState(State state) { this->state_ = state; }

private:
    EventLoop* loop_;

    ReadFunction readFunction_;
    ComputFunction computFunction_;
    WriteFunction writeFunction_;
    ConnectionCallback connectionCallback_;

    Buffer inputBuffer_;
    Buffer computOverBuffer_;
    Buffer outputBuffer_;

    std::shared_ptr<ThreadPool> threadPool_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddr localAddr_;
    InetAddr peerAddr_;

    int connectionId_;
    State state_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

}

#endif