#include "AsyncLog.h"
#include <chrono>
#include <iostream>

using namespace grt;
using namespace log;

const size_t AsyncLog::bufferSize;

AsyncLog::AsyncLog(int flushInterval)
    : running_(false) ,
      mutex_() ,
      cond_() ,
      currentBuffer_(new Buffer) ,
      nextBuffer_(new Buffer) ,
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
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    // all the buffer_ wait to be writeen
    BufferVector writeBufferVec;

    while (this->running_) {
        assert(newBuffer1);
        assert(newBuffer2);
        assert(writeBufferVec.empty());
        {
            std::lock_guard<std::mutex> locker(this->mutex_);
            // if no full buffer_
            if (this->vecBuffer_.empty()) {
                std::unique_lock lock(this->cond_mutex_);
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

        if (!newBuffer1) {
            assert(!writeBufferVec.empty());
            newBuffer1 = std::move(writeBufferVec.back());
            writeBufferVec.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer1) {
            assert(!writeBufferVec.empty());
            newBuffer2 = std::move(writeBufferVec.back());
            writeBufferVec.pop_back();
            newBuffer1->reset();
        }

        writeBufferVec.clear();
    }
}

void AsyncLog::start() {
    this->running_ = true;
    this->thread_ = std::thread(std::bind(&AsyncLog::threadFunc , this));
    this->thread_.detach();
}