#include "Log.h"
#include "../Base/Timeoperator.h"

#include <stdarg.h>


grt::log::Log LOG_INFO(grt::log::cInfo);
grt::log::Log LOG_DEBUG(grt::log::cDebug);
grt::log::Log LOG_WARN(grt::log::cWarning);
grt::log::Log LOG_ERROR(grt::log::cError);
grt::log::Log LOG_CRIT(grt::log::cCritical);

void LOG(grt::log::Level level , const char* format , ...) {
    va_list ap;
    va_start(ap , format);
    char buf[grt::log::logMaxLength];
    vsnprintf(buf , grt::log::logMaxLength - 1 , format , ap);

    auto find_res = grt::log::LogFile.find(level);
    assert(find_res != grt::log::LogFile.end());

    if (find_res->second) {
        if (level == grt::log::cDebug) {
            LOG_DEBUG << buf;
        }
        else if (level == grt::log::cInfo) {
            LOG_INFO << buf;
        }
        else if (level == grt::log::cWarning) {
            LOG_WARN << buf;
        }
        else if (level == grt::log::cError) {
            LOG_ERROR << buf;
        }
        else if (level == grt::log::cCritical) {
            LOG_CRIT << buf;
        }
        else {
            LOG_WARN << "Unknown Log Level";
        }
    }
}


namespace grt
{

namespace log
{

int initLogFileOnce_once = 0;

void initLogFileOnce() {
    if (log::initLogFileOnce_once == 0) {
        assert(log::LogFile.empty());
        log::LogFile.insert(std::make_pair(cDebug , false));
        log::LogFile.insert(std::make_pair(cInfo , true));
        log::LogFile.insert(std::make_pair(cWarning , false));
        log::LogFile.insert(std::make_pair(cError , true));
        log::LogFile.insert(std::make_pair(cCritical , true));
        log::initLogFileOnce_once = 1;
    }
}

void setLogLevelPermission(Level level , bool on) {
    assert(!log::LogFile.empty());
    auto find_res = log::LogFile.find(level);
    if (find_res != log::LogFile.end()) {
        find_res->second = on;
    }
}

Log::Log(Level level) 
    : level_(level)
{
    log::initLogFileOnce();
    log::start();
}

Log::~Log() {

}

std::string Log::levelToString() {
    std::string res;
    if (this->level_ == cInfo) { res.assign("INFO"); }
    if (this->level_ == cDebug) { res.assign("DEBUG"); }
    if (this->level_ == cWarning) { res.assign("WARN"); }
    if (this->level_ == cError) { res.assign("ERROR"); }
    if (this->level_ == cCritical) { res.assign("CRIT"); }
    return res;
}

Log& Log::operator<< (std::string& data) {
    auto find_res = log::LogFile.find(this->level_);
    assert(find_res != log::LogFile.end());
    if (find_res->second) {
        char resdata[log::logMaxLength] = {0};
        snprintf(resdata , sizeof(resdata) , "%s : %s %s\n" , this->levelToString().c_str() , grt::base::nowTime() , data.c_str());
        log::asynclog.append(resdata , sizeof(resdata));
    }
    return *this;
}
Log& Log::operator<< (const char* data) {
    auto find_res = log::LogFile.find(this->level_);
    assert(find_res != log::LogFile.end());
    if (find_res->second) {
        char resdata[log::logMaxLength] = {0};
        snprintf(resdata , sizeof(resdata) , "%s : %s %s\n" , this->levelToString().c_str() , grt::base::nowTime() , data);
        log::asynclog.append(resdata , sizeof(resdata));
    }
    return *this;
}

void start() {
    if (!log::asynclog.isrunning()) {
        log::asynclog.start();
    }   
}

}
}