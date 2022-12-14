#include "AsyncLog.h"
#include "Log.h"

#include <chrono>
#include <iostream>

namespace grt
{

namespace log
{

const size_t AsyncLog::bufferSize;

AsyncLog::AsyncLog(int flushInterval)
    : running_(false) ,
      mutex_() ,
      cond_() ,
      currentBuffer_(new Buffer(bufferSize)) ,
      nextBuffer_(new Buffer(bufferSize)) ,
      vecBuffer_() ,
      thread_() ,
      flushInterval_(flushInterval)
{
    
}

AsyncLog::~AsyncLog() {}

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

void AsyncLog::threadFunc() {
    assert(this->running_ == true);
    BufferPtr newBuffer1(new Buffer(this->bufferSize));
    BufferPtr newBuffer2(new Buffer(this->bufferSize));
    // all the buffer_ wait to be writeen
    BufferVector writeBufferVec;

    while (this->running_) {
        assert(newBuffer1 && newBuffer1->readableBytes() == 0 && newBuffer1->writableBytes() == this->bufferSize);
        assert(newBuffer2 && newBuffer2->readableBytes() == 0 && newBuffer2->writableBytes() == this->bufferSize);
        assert(writeBufferVec.empty());

        {
            std::unique_lock lock(this->mutex_);
            // if no full buffer_
            if (this->vecBuffer_.empty()) {
                // wait for a while and flush buffer_;
                this->cond_.wait_for(lock , std::chrono::seconds(this->flushInterval_));
            }
            this->vecBuffer_.push_back(std::move(this->currentBuffer_));
            this->currentBuffer_ = std::move(newBuffer1);
            // swap them and writeBufferVec has all log to be written
            this->vecBuffer_.swap(writeBufferVec);
            // if nextBuffer has been used in append function
            if (!this->nextBuffer_) {
                this->nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!writeBufferVec.empty());

        // ouput all logs
        for (const auto& buffer : writeBufferVec) {
            std::cout << buffer->retrieveAllAsString();
        }

        // refill newBUffer1 and newBuffer2
        if (!newBuffer1) {
            assert(!writeBufferVec.empty());
            newBuffer1 = std::move(writeBufferVec.back());
            writeBufferVec.pop_back();
            newBuffer1->reset(this->bufferSize);
        }

        if (!newBuffer1) {
            assert(!writeBufferVec.empty());
            newBuffer2 = std::move(writeBufferVec.back());
            writeBufferVec.pop_back();
            newBuffer1->reset(this->bufferSize);
        }

        writeBufferVec.clear();
    }
}

void AsyncLog::start() {
    this->running_ = true;
    this->thread_ = std::thread(std::bind(&AsyncLog::threadFunc , this));
    this->thread_.detach();
}

    
}
}
