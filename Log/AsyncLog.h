#ifndef GRT_LOG_ASYNCLOG_H
#define GRT_LOG_ASYNCLOG_H

#include "../Base/Noncopyable.h"
#include "../Buffer.h"
#include "../Base/Mutexoperator.h"

#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>
#include <assert.h>
#include <atomic>

namespace grt
{
namespace log
{

class AsyncLog : base::noncopyable {
public:

    AsyncLog(int flushInterval = 3);
    ~AsyncLog();

    // append data to currentBuffer_
    void append(const char* logdata , int len);

    // start to work
    void start();

    bool isrunning() { return this->running_; }

private:
    void threadFunc();

    const static size_t bufferSize = 4000000;

    typedef std::unique_ptr<Buffer> BufferPtr;
    typedef std::vector<BufferPtr> BufferVector;

    // the Buffer current will write
    BufferPtr currentBuffer_ GUARDED_BY(mutex_);
    // if currentBuffer_ is full , write nextBuffer_
    BufferPtr nextBuffer_ GUARDED_BY(mutex_);
    // stored all full Buffers
    BufferVector vecBuffer_ GUARDED_BY(mutex_);

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_ GUARDED_BY(mutex_);

    std::atomic<bool> running_;
    // the time to flush the buffer if no buffer is full
    const int flushInterval_;
};

}
}
#endif