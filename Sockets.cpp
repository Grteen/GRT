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

int bind(int sockfd , const struct sockaddr* addr) {
    int res = ::bind(sockfd , addr , static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    if (res < 0) {
        LOG(CRIT , "sockets::bind");
    }
    return res;
}

void listen(int sockfd) {
    int res = ::listen(sockfd , SOMAXCONN);
    if (res < 0) {
        LOG(CRIT , "sockets::listen");
    }
}

void close(int sockfd) {
    if (::close(sockfd) < 0) {
        LOG(ERROR , "sockets::close");
    }
}

int tcp_listen(const char* host , const char* port , socklen_t* addrlenp) {
    struct addrinfo hints , *res , *ressave;
    int n = 0;
    const int on = 1;
    int listenfd = 0;
    bzero(&hints , sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if ((n = getaddrinfo(host , port , &hints , &res)) != 0) {
        LOG(CRIT , "sockets::tcp_listen");
    }

    ressave = res;

    do {
        listenfd = sockets::createNonblockSocket(res->ai_family);
        if (listenfd < 0) {
            // error try next one
            continue;       
        }
        setsockopt(listenfd , SOL_SOCKET , SO_REUSEADDR , &on , sizeof(on));
        if (sockets::bind(listenfd , res->ai_addr) == 0) {
            // success
            break;
        }

        sockets::close(listenfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL) {
        LOG(CRIT , "tcp_listen error for %s , %s" , host , port);
    }

    sockets::listen(listenfd);
    if (addrlenp) {
        *addrlenp = res->ai_addrlen;
    }

    freeaddrinfo(ressave);

    return listenfd;
}

}
}