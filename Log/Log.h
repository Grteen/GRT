#ifndef GRT_LOG_LOG_H
#define GRT_LOG_LOG_H
#include <iostream>
#include <string>
#include <string.h>

#include "AsyncLog.h"

namespace grt
{
namespace log
{

// only used to ouput the string and char* data
class Log {
public:

    Log& operator<< (std::string& data);
    Log& operator<< (const char* data);
    
    static void start();

private:
    static AsyncLog asynclog_;
};
}
}
#endif