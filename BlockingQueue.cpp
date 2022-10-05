#include "BlockingQueue.h"
#include "Log/Log.h"

namespace grt
{

BlockingQueue::BlockingQueue()
    : queue_() ,
      bufQueue_() ,
      moving_(false)
{

}

BlockingQueue::~BlockingQueue() {

}

BlockingQueue::ComputFunction BlockingQueue::take() {
    BlockingQueue::ComputFunction cmpfun;
    {
        std::lock_guard<std::mutex> locker(this->mutex_);

        if (this->queue_.empty() && !this->bufQueue_.empty()) {
            this->moving_ = true;
            std::unique_lock<std::mutex> lock(this->bufMutex_);
            this->moveBuf();
            this->moving_ = false;
            this->cond_.notify_all();
        }

        if (!this->queue_.empty()) {
            cmpfun = this->queue_.front();
            this->queue_.pop();
        }
        else {
            LOG(WARN , "BlockingQueue take empty queue");
        }
    }

    return cmpfun;
}

void BlockingQueue::put(const ComputFunction& func) {
    {
        std::unique_lock<std::mutex> locker(this->bufMutex_);
        while (this->moving_) {
            this->cond_.wait(locker);
        }
        this->bufQueue_.push(std::move(func));
    }
}

void BlockingQueue::moveBuf() {
    LOG(DEBUG , "BlockingQueue::moveBuf begin");
    while (!this->bufQueue_.empty()) {
        this->queue_.push(std::move(this->bufQueue_.front()));
        this->bufQueue_.pop();
    }
}

bool BlockingQueue::empty() {
    return this->queue_.empty() && this->bufQueue_.empty();
}

}