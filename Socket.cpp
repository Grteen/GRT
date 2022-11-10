#include "Socket.h"
#include "Sockets.h"
#include "Log/Log.h"
#include "InetAddr.h"

#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/tcp.h>

namespace grt
{

Socket::Socket(int sockfd) 
    : sockfd_(sockfd)
{

}

Socket::~Socket() {
    sockets::close(this->sockfd_);
}


void Socket::bindAddress(const InetAddr& addr) {
    sockets::bind(this->sockfd_ , addr.getSockAddrInet());
}

void Socket::listen() {
    sockets::listen(this->sockfd_);
}

int Socket::accept(InetAddr* peeraddr) {
    struct sockaddr_in newAddr;
    bzero(&newAddr , sizeof(newAddr));
    int connfd = sockets::accept(this->sockfd_ , &newAddr);

    if (connfd >= 0) {
        peeraddr->setSockAddrInet(newAddr);
    }

    return connfd;
}   

void Socket::shutdownWrite() {
    sockets::shutdownWrite(this->sockfd_);
}

void Socket::setTcpNoDelay(bool on) {
    int val = on ? 1 : 0;
    setsockopt(this->sockfd_ , IPPROTO_TCP , TCP_NODELAY , &val , sizeof(val));
}

}
