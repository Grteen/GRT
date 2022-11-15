#ifndef GRT_THREADPOOL_H
#define GRT_THREADPOOL_H

#include "../Base/Noncopyable.h"
#include "../Base/Mutexoperator.h"

#include <memory>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace grt
{

class BlockingQueue;
class TcpConnection;

// computing ThreadPoll only used to call computing function
class ThreadPool : base::noncopyable {
public:
    typedef std::function<void ()> ComputFunction;

    ThreadPool() = delete;
    explicit ThreadPool(int threadNumber);
    ~ThreadPool();

    void start();

    void puttask(const ComputFunction& cf);

    bool checkStart() { return this->started_; }

    void setThreadNum(int threadnum) { this->threadNumber_ = threadnum; }

    bool isEmpty() { return this->threads_.empty(); }

private:
    // threads_ will call this function
    void thrfunc();

private:
    int threadNumber_;
    bool started_;

    std::unique_ptr<BlockingQueue> blockingQueue_;
    std::vector<std::thread> threads_;

    std::condition_variable cond_ GUARDED_BY(this->mutex_);
    std::mutex mutex_;
};

}


#endif