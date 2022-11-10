#ifndef GRT_SOCKET_H
#define GRT_SOCKET_H

#include <string>

namespace grt
{

class InetAddr;

// encapsulation of sockfd
class Socket {
public:
    Socket(int sockfd);
    ~Socket();

    // protocol independent bind
    void bindAddress(const InetAddr& localaddr);

    void listen();

    int accept(InetAddr* peeraddr);

    int sockfd() { return this->sockfd_; }

    void shutdownWrite();

    void setTcpNoDelay(bool on);
private:

private:
    int sockfd_;
};

}

#endif
