#include "ThreadPool.h"
#include "BlockingQueue.h"
#include "TcpConnection.h"
#include "Log/Log.h"

namespace grt
{

ThreadPool::ThreadPool(int threadNumber) 
    : blockingQueue_(new BlockingQueue) ,
      threads_() ,
      threadNumber_(threadNumber) ,
      started_(false)
{

}

ThreadPool::~ThreadPool() {

}

void ThreadPool::start() {
    if (!this->started_) {
        for (int i = 0 ; i < this->threadNumber_ ; i++) {
            std::thread t(&ThreadPool::thrfunc , this);
            this->threads_.push_back(std::move(t));
        }

        this->started_ = true;
    }
    else {
        LOG(WARN , "thread pool has been started");
    }
}

void ThreadPool::puttask(const ComputFunction& cf) {
    if (this->blockingQueue_->empty()) {
        std::unique_lock<std::mutex> locker(this->mutex_);
        this->blockingQueue_->put(cf);
        this->cond_.notify_all();
    }
    else {
        this->blockingQueue_->put(cf);
    }
}

void ThreadPool::thrfunc() {
    while (this->started_) {
        {
            std::unique_lock<std::mutex> locker(this->mutex_);
            while (this->blockingQueue_->empty()) {
                this->cond_.wait(locker);
            }
        }
        auto func = this->blockingQueue_->take();
        if (func) {
            func();
        }
    }
}


}