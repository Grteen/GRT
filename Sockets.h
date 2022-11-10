#ifndef GRT_SOCKETS_H
#define GRT_SOCKETS_H

#include <arpa/inet.h>
#include <iostream>

namespace grt
{

namespace sockets
{

inline uint64_t hostToNetwork64(uint64_t host64) {
   return htobe64(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32) {
   return htonl(host32);
}

inline uint16_t hostToNetwork16(uint16_t host16) {
   return htons(host16);
}

inline uint64_t networkToHost64(uint64_t net64) {
   return be64toh(net64);
}

inline uint32_t networkToHost32(uint32_t net32) {
   return ntohl(net32);
}

inline uint16_t networkToHost16(uint16_t net16) {
    return ntohs(net16);
}

// all socket-operator wrapper function
ssize_t write(int sockfd , const void* buf , size_t size);
ssize_t read(int sockfd , void* buf , size_t size);

// create a nonblocking socket 
int createNonblockSocket(sa_family_t family);

void bind(int sockfd , const struct sockaddr_in& addr);
void listen(int sockfd);
void close(int sockfd);

int accept(int sockfd , struct sockaddr_in* addr);

void toHostPort(char * buf , size_t size , const struct sockaddr_in& addr);

void fromHostPort(const char * ip , uint16_t port , struct sockaddr_in* addr);

struct sockaddr_in getLocalAddr(int sockfd);

void shutdownWrite(int sockfd);

}

}
#endif