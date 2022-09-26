#ifndef GRT_LOG_LOG_H
#define GRT_LOG_LOG_H
#include <iostream>
#include <string>

namespace grt
{
namespace log
{

// only used to ouput the string and char* data
class Log {
public:
    Log& operator<< (std::string data) {
        std::cout << data;
        return *this;
    }

    Log& operator<< (const char* data) {
        std::cout << data;
        return *this;
    }
private:
};
}
}
#endif