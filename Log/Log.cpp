#include "Log.h"

using namespace grt;
using namespace log;

AsyncLog Log::asynclog_;

Log& Log::operator<< (std::string& data) {
    this->asynclog_.append(data.c_str() , data.size());
    return *this;
}
Log& Log::operator<< (const char* data) {
    this->asynclog_.append(data , ::strlen(data));
    return *this;
}

void Log::start() {
    if (!Log::asynclog_.isrunning()) {
        Log::asynclog_.start();
    }
}