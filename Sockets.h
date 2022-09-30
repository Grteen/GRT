#ifndef GRT_SOCKETS_H
#define GRT_SOCKETS_H

#include <arpa/inet.h>

namespace grt
{

namespace sockets
{

// all socket-operator wrapper function
ssize_t write(int sockfd , const void* buf , size_t size);
ssize_t read(int sockfd , void* buf , size_t size);

// create a nonblocking socket 
int createNonblockSocket(sa_family_t family);

int bind(int sockfd , const struct sockaddr* addr);
void listen(int sockfd);
void close(int sockfd);

// protocol independent listen
int tcp_listen(int sockfd , const char* host , const char* port , socklen_t* addrlenp);


}

}
#endif