#ifndef GRT_LOG_LOG_H
#define GRT_LOG_LOG_H
#include <iostream>
#include <string>
#include <string.h>
#include <unordered_map>

#include "AsyncLog.h"

namespace grt
{
namespace log
{

const int logMaxLength = 512;
// level of log 
enum Level {cDebug , cInfo , cWarning , cError , cCritical , };

// the enable of log output which have different levels
static std::unordered_map<Level , bool> LogFile;
// set enable of log output with target levels
void setLogLevelPermission(Level level , bool on);

// initialize the LogFile only once
void initLogFileOnce();
extern int initLogFileOnce_once;

// start the asynclog_
void start();
static AsyncLog asynclog;

// only used to ouput the string and char* data
class Log {
public:
    Log() = delete;

    Log(Level level);

    ~Log();

    Log& operator<< (std::string& data);
    Log& operator<< (const char* data);

private:
    std::string levelToString();
    Level level_;
};

}
}

extern grt::log::Log LOG_INFO;
extern grt::log::Log LOG_DEBUG;
extern grt::log::Log LOG_WARN;
extern grt::log::Log LOG_ERROR;
extern grt::log::Log LOG_CRIT;

#define DEBUG grt::log::cDebug
#define INFO grt::log::cDeinfo
#define WARN grt::log::cWarning
#define ERROR grt::log::cError
#define CRIT grt::log::cCritical

void LOG(grt::log::Level level , const char* format , ...);

#endif