#include "Channel.h"
#include "EventLoop.h"
#include "Log/Log.h"

#include <sys/epoll.h>

namespace grt
{

// all the events
const int Channel::cNoneEvent = 0;
const int Channel::cReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::cWriteEvent = EPOLLOUT;

// condition
const int Channel::cNew = -1;
const int Channel::cAdd = 1;
const int Channel::cDel = 2;

Channel::Channel(EventLoop* loop , int fd)
    : loop_(loop) ,
      fd_(fd) ,
      events_(0) ,
      revents_(0) ,
      condition_(cNew) ,
      eventHandling_(false)
{

}

Channel::~Channel() {
    assert(this->eventHandling_ == false);
}

int count = 0;
void Channel::handleEvent() {
    this->eventHandling_ = true;
    // error event
    if (this->revents_ & EPOLLERR) {
        if (this->errorCallback_) this->errorCallback_();
    }
    // close event
    if ((this->revents_ & EPOLLHUP) && !(this->revents_ & EPOLLIN)) {
        LOG(WARN , "Channel::handleEvent get POLLHUB");
        if (this->closeCallback_) this->closeCallback_();
    }
    // read event
    if (this->revents_ & (EPOLLPRI | EPOLLHUP | EPOLLIN)) {
        if (this->readCallback_) this->readCallback_();
    }
    // write event
    if (this->revents_ & EPOLLOUT) {
        if (this->writeCallback_) this->writeCallback_();
    }

    this->eventHandling_ = false;
}

void Channel::update() {
    // call ownerloop to update
    this->loop_->updateChannel(this);
}

std::string Channel::eventsToString(int fd , int events) {
    std::string res;
    if (events & EPOLLIN)
        res = res + "EPOLLIN ";
    if (events & EPOLLPRI)
        res = res + "EPOLLPRI ";
    if (events & EPOLLOUT)
        res = res + "EPOLLOUT ";
    if (events & EPOLLHUP)
        res = res + "EPOLLHUP ";
    if (events & EPOLLERR)
        res = res + "EPOLLERR ";
    if (events & EPOLLRDHUP)
        res = res + "EPOLLRDHUP ";
    if (events & EPOLLET)
        res = res + "EPOLLET ";
    
    return res;
}

}