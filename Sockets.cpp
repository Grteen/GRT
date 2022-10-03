#include "Sockets.h"
#include "Log/Log.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>

namespace grt
{

namespace sockets
{

void toHostPort(char * buf , size_t size , const struct sockaddr_in& addr) {
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET , &addr.sin_addr , host , sizeof(host));
    uint16_t port = sockets::networkToHost16(addr.sin_port);
    snprintf(buf , size , "%s:%u" , host , port);
}

void fromHostPort(const char* ip , uint16_t port , struct sockaddr_in* addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET , ip , &addr->sin_addr) <= 0) {
        LOG(ERROR , "sockets::fromHostPort");
    }
}

ssize_t write(int sockfd , const void* buf , size_t size) {
    return ::write(sockfd , buf , size);
}

ssize_t read(int sockfd , void* buf , size_t size) {
    return ::read(sockfd , buf , size);
}

int createNonblockSocket(sa_family_t family) {
    int sockfd = ::socket(family , SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC , IPPROTO_TCP);
    if (sockfd < 0) {
        LOG(CRIT , "sockets::createNonblockSocket");
    }

    return sockfd;
}

void listen(int sockfd) {
    int res = ::listen(sockfd , SOMAXCONN);
    if (res < 0) {
        LOG(CRIT , "sockets::listen");
    }
}

void bind(int sockfd , const struct sockaddr_in& addr) {
    int res = ::bind(sockfd , (sockaddr*)&addr , sizeof(addr));
    if (res < 0) {
        LOG(CRIT , "sockets::bind");
    }
}

int accept(int sockfd , struct sockaddr_in* addr) {
    socklen_t addrlen = sizeof(*addr);
    int connfd = ::accept4(sockfd , (sockaddr*)&addr , &addrlen , SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        LOG(ERROR , "sockets::accept");
    }
    return connfd;
}

void close(int sockfd) {
    if (::close(sockfd) < 0) {
        LOG(ERROR , "sockets::close");
    }
}

struct sockaddr_in getLocalAddr(int sockfd) {
    struct sockaddr_in localaddr;
    bzero(&localaddr , sizeof(localaddr));
    socklen_t addrlen = sizeof(localaddr);
    if (::getsockname(sockfd , (sockaddr*)&localaddr , &addrlen) < 0) {
        LOG(ERROR , "sockets::getLocalAddr");
    }
    return localaddr;
}


}
}