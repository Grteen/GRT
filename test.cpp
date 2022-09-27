#define GRT_DEBUG
#include "Buffer.h"
#include "Log/Log.h"
#include <iostream>
int main(void) {
    grt::log::setLogLevelPermission(grt::log::cDebug , true);
    LOG_INFO << "INFO";
    LOG_ERROR << "ERROR";
    LOG_WARN << "WARN";
    LOG_CRIT << "CRIT";
    LOG_DEBUG << "DEBUG";
    while(1);
}