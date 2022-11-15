#ifndef GRT_BLOCKINGQUEUE_H
#define GRT_BLOCKINGQUEUE_H

#include "../Base/Noncopyable.h"
#include "../Base/Mutexoperator.h"

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace grt
{

class Buffer;
class TcpConnection;

class BlockingQueue : base::noncopyable {
public:
    typedef std::function<void ()> ComputFunction;

    BlockingQueue();
    ~BlockingQueue();

    ComputFunction take();
    void put(const ComputFunction& func);

    bool empty();
private:
    void moveBuf();

private:
    bool moving_;

    std::mutex mutex_;
    // consumer will take function from it
    std::queue<ComputFunction> queue_ GUARDED_BY(this->mutex_);

    std::mutex bufMutex_;
    std::condition_variable cond_;
    // producer will put function to it
    std::queue<ComputFunction> bufQueue_ GUARDED_BY(this->bufMutex_);
};

}

#endif