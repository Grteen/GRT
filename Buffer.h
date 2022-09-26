#ifndef GRT_BUFFER_H
#define GRT_BUFFER_H

#include <vector>
#include <sys/types.h>
#include <algorithm>
#include <assert.h>
#include <string>

namespace grt
{

class Buffer {
public:
    #ifdef GRT_DEBUG
    std::vector<char> returnBuffer() {
        return this->buffer_;
    }
    #endif
public:
    // prepend bytes in buffer_
    static const size_t cPrepend = 8;
    // init size of buffer_
    static const size_t cInitSize = 1024;

    explicit Buffer(size_t initSize = cInitSize);

    // swap with another Buffer
    void swap(Buffer& rhs);

    // return the size of bytes which can be read now
    size_t readableBytes() const {
        return this->writeIndex_ - this->readIndex_;
    }

    // return the size of bytes which can be wroten now
    size_t writableBytes() const {
        return this->buffer_.size() - this->writeIndex_;
    }

    // return the prepend size
    size_t prependableBytes() const {
        return this->readIndex_;
    }

    // return the first address can be readof of this buffer_
    const char* readAddr() const {
        return this->begin() + this->readIndex_;
    }

    // return the first address can be wroten of this buffer_ 
    char* writeAddr() {
        return this->begin() + this->writeIndex_;
    }

    const char* writeAddr() const {
        return this->begin() + this->writeIndex_;
    }

    // find \r\n and return the address of buffer_
    const char* findCRLF() const {
        const char* crlf = std::search(this->readAddr() , this->writeAddr() , cCRLF , cCRLF + 2);
        return crlf == this->writeAddr() ? NULL : crlf;
    }

    // retrieve the data in buffer_ and (delete) them
    // if you want to get the data , use retrieveAsString
    void retrieve(size_t len) {
        assert(len <= this->readableBytes());
        if (len < readableBytes()) {
            this->readIndex_ += len;
        }
        else {
            this->retrieveAll();
        }
    }

    void retrieveAll() {
        this->readIndex_ = cPrepend;
        this->writeIndex_ = cPrepend;
    }

    void retrieveUntil(const char* end) {
        assert(this->readAddr() <= end);
        assert(end <= this->writeAddr());
        this->retrieve(end - this->readAddr());
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= this->readableBytes());
        std::string res(this->readAddr() , len);
        this->retrieve(len);
        return res;
    }

    std::string retrieveAllAsString() {
        return this->retrieveAsString(this->readableBytes());
    }

    // ensure buffer_ has enough place to write
    void ensureWritableBytes(size_t len);

    // when buffer_'s writing is over , change the writeIndex_
    void writeOver(size_t len) {
        assert(len <= this->writableBytes());
        this->writeIndex_ += len;
    }

    // append the data to buffer_
    void append(const char* data , size_t len) {
        this->ensureWritableBytes(len);
        std::copy(data , data + len , this->writeAddr());
        this->writeOver(len);
    }

    void append(const void* data , size_t len) {
        this->append(static_cast<const char*>(data) , len);
    }

    // add data in prepend
    void prepend(const void* data , size_t len) {
        assert(len <= this->prependableBytes());
        this->readIndex_ -= len;
        const char* cast_data = static_cast<const char*>(data);
        std::copy(cast_data , cast_data + len , this->begin() + this->readIndex_);
    }

    // shrink the buffer_'s size and capacity
    void shrink(size_t reserve) {
        Buffer others;
        std::string data(this->readAddr() , static_cast<int>(this->readableBytes()));
        others.ensureWritableBytes(this->readableBytes() + reserve);
        others.append(data.c_str() , sizeof(data.c_str()));
        this->swap(others);
        this->buffer_.shrink_to_fit();
    }

private:
    // return buffer_ begin address
    char* begin() {
        return &*(this->buffer_.begin());
    }

    const char* begin() const {
        return &*(this->buffer_.begin());
    }
    
    void expandSpace(size_t len);

    void moveDataToFront();

private:
    // use vector<char> as buffer
    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;

    static const char cCRLF[];
};
}
#endif