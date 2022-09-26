#include "AsyncLog.h"

using namespace grt;
using namespace log;

const size_t AsyncLog::bufferSize;

AsyncLog::AsyncLog()
    : running_(false) ,
      mutex_() ,
      cond_() ,
      currentBuffer_(new Buffer) ,
      nextBuffer_(new Buffer) ,
      vecBuffer_() ,
      thread_(std::bind(&AsyncLog::threadFunc , this))
{
    
}

void AsyncLog::append(const char* logdata , int len) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    // rest space is enough
    if (this->currentBuffer_->writableBytes() > len) {
        this->currentBuffer_->append(logdata , len);
    }
    // rest space is not enough
    else {
        // put the full buffer into vecBuffer_
        this->vecBuffer_.push_back(std::move(this->currentBuffer_));
        // if have another buffer which is null
        if (this->nextBuffer_) {
            this->currentBuffer_ = std::move(this->nextBuffer_);
        }
        // no another buffer
        else {
            // create a new Buffer and make currentBuffer_ point to it
            this->currentBuffer_.reset(new Buffer(this->bufferSize));
        }
        this->currentBuffer_->append(logdata , len);
        this->cond_.notify_one();
    }
}