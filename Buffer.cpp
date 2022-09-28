#include "Buffer.h"

namespace grt
{

const char Buffer::cCRLF[] = "\r\n";

Buffer::Buffer(size_t initSize)
    : buffer_(cPrepend + initSize) ,
      readIndex_(cPrepend) ,
      writeIndex_(cPrepend)
{
    assert(this->readableBytes() == 0);
    assert(this->writableBytes() == initSize);
    assert(this->prependableBytes() == cPrepend);
}

void Buffer::swap(Buffer& rhs) {
    // swap the data in vector<char>
    this->buffer_.swap(rhs.buffer_);
    // swap the index
    std::swap(this->readIndex_ , rhs.readIndex_);
    std::swap(this->writeIndex_ , rhs.writeIndex_);
}

void Buffer::ensureWritableBytes(size_t len) {
    if (this->writableBytes() < len) {
        this->expandSpace(len);
    }
    assert(this->writableBytes() >= len);
}

void Buffer::expandSpace(size_t len) {
    // no rest size
    if (this->writableBytes() + this->prependableBytes() < len + cPrepend) {
        this->moveDataToFront();
        this->buffer_.resize(this->writeIndex_ + len);
    }
    // rest size is enough
    else {
        this->moveDataToFront();
    }
}

void Buffer::moveDataToFront() {
    // move readable data to the front
    assert(cPrepend <= this->readIndex_);
    size_t readablebytes = this->readableBytes();
    // this move equal to std::copy
    std::move(this->begin() + this->readIndex_ ,
              this->begin() + this->writeIndex_ ,
              this->begin() + cPrepend);
    this->readIndex_ = cPrepend;
    this->writeIndex_ = this->readIndex_ + readablebytes;
    assert(readablebytes == this->readableBytes());
}

}