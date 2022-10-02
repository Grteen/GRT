#ifndef GRT_TCPCONNECTION_H
#define GRT_TCPCONNECTION_H

#include "./Base/Noncopyable.h"
#include "Callback.h"
#include "Buffer.h"

#include <memory>

namespace grt
{

class EventLoop;
class ThreadPool;

class TcpConnection : base::noncopyable ,
                      public std::enable_shared_from_this<TcpConnection> 
{
public:
    TcpConnection(EventLoop* loop);
    ~TcpConnection();

    void setReadFunction(const ReadFunction& rf) { this->readFunction_ = rf; }
    void setWriteFunction(const WriteFunction& wf) { this->writeFunction_ = wf; }
    void setComputFunction(const ComputFunction& cf) { this->computFunction_ = cf; }

    void onMessageCallback();
    void readOverCallback(Buffer& computBuf , Buffer& outputBuf);
    void computOverCallback(Buffer& outputBuf);
private:

private:
    EventLoop* loop_;

    ReadFunction readFunction_;
    ComputFunction computFunction_;
    WriteFunction writeFunction_;

    Buffer inputBuffer_;
    Buffer computOverBuffer_;
    Buffer outputBuffer_;

    std::unique_ptr<ThreadPool> threadPool_;
};

}

#endif