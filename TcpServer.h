#ifndef GRT_TCPSERVER_H
#define GET_TCPSERVER_H

#include "./Base/Noncopyable.h"
#include "TcpConnection.h"
#include "Callback.h"

#include <memory>
#include <unordered_map>

namespace grt
{

class EventLoop;
class Acceptor;

class TcpServer : base::noncopyable {
public:
    TcpServer() = delete;
    TcpServer(EventLoop* loop , const InetAddr& listenAddr);
    ~TcpServer();

    // read function will be called in EventLoop's thread
    void setReadFunction(const ReadFunction& rf) { this->readFunction_ = rf; }
    // write function will be called in EventLoop's thread
    void setWriteFunction(const WriteFunction& wf) { this->writeFunction_ = wf; }
    // comput function will be called in computingThread Pool
    void setComputFunction(const ComputFunction& cf) { this->computFunction_ = cf; }

    void setConnectionCallback(const ConnectionCallback& cc) { this->connectionCallback_ = cc; }

    void start();
private:
    void handleConnection(int sockfd , const InetAddr& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);

private:
    typedef std::unordered_map<int , TcpConnectionPtr> connectionMap;
    // acceptor_'s loop
    EventLoop* loop_;       
    std::unique_ptr<Acceptor> acceptor_;

    bool started_;
    connectionMap connections_;

    // set by user and these function will be used to set TcpConnection's function
    ReadFunction readFunction_;
    ComputFunction computFunction_;
    WriteFunction writeFunction_;
    ConnectionCallback connectionCallback_;

    int currentConnId_;
    std::shared_ptr<ThreadPool> threadPool_;
};

}

#endif
