#ifndef GRT_THREADPOOL_H
#define GRT_THREADPOOL_H

#include "Base/Noncopyable.h"

#include <memory>
#include <vector>
#include <thread>
#include <functional>

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

private:
    // threads_ will call this function
    void thrfunc();

private:
    int threadNumber_;
    bool started_;

    std::unique_ptr<BlockingQueue> blockingQueue_;
    std::vector<std::thread> threads_;
};

}


#endif