#include "InetAddr.h"
#include "Sockets.h"

#include <string.h>
#include <iostream>
namespace grt
{

InetAddr::InetAddr(uint16_t port) {
    bzero(&this->addr_ , sizeof(this->addr_));
    this->addr_.sin_family = AF_INET;
    this->addr_.sin_addr.s_addr = sockets::hostToNetwork32(INADDR_ANY);
    this->addr_.sin_port = sockets::hostToNetwork16(port);
}

InetAddr::InetAddr(const std::string& ip , uint16_t port) {
    bzero(&this->addr_ , sizeof(this->addr_));
    sockets::fromHostPort(ip.c_str() , port , &this->addr_);
}

std::string InetAddr::toHostPort() const {
    char buf[32];
    sockets::toHostPort(buf , sizeof(buf) , this->addr_);
    return buf;
}

}