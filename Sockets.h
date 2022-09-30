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
}

}
#endif