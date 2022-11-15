#ifndef GRT_INETADDR_H
#define GRT_INETADDR_H

#include <netinet/in.h>
#include <string>

namespace grt
{
// wrapper of sockaddr_in
class InetAddr {
public:
    explicit InetAddr(uint16_t port);
    explicit InetAddr(const std::string& ip , uint16_t port);
    explicit InetAddr(const struct sockaddr_in& addr);

    const struct sockaddr_in& getSockAddrInet() const {
        return this->addr_;
    }

    void setSockAddrInet(const struct sockaddr_in& addr) {
        this->addr_ = addr;
    }
    
    // return the port in string
    std::string toHostPort() const;
private:

private:
    struct sockaddr_in addr_;
};

}

#endif