#ifndef GRT_ACCEPTOR_H
#define GRT_ACCEPTOR_H

#include "./Base/Noncopyable.h"
#include "Channel.h"
#include "Socket.h"

#include <functional>
#include <string>

namespace grt
{

class InetAddr;
class EventLoop;

// used to accept new connection
class Acceptor : base::noncopyable {
public:
    typedef std::function<void (int sockfd , const InetAddr&)> newConnectionCallback;

    Acceptor(EventLoop* loop , const InetAddr& addr);

    void listen();
    bool listening() { return this->listening_; }

    void setNewConnectionCallback(const newConnectionCallback& cb) { this->connectionCallback_ = cb; } 
private:
    void handleRead();

private:
    EventLoop* loop_;
    Socket acceptsockfd_;
    Channel acceptChannel_;
    // set by TcpServer
    newConnectionCallback connectionCallback_;
    bool listening_;
};

}
#endif