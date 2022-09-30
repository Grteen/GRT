#include "Sockets.h"
#include "Log/Log.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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

}
}