#ifndef GRT_CHANNEL_H
#define GRT_CHANNEL_H

#include "Base/Noncopyable.h"

#include <functional>
#include <string>

namespace grt
{

class EventLoop;

// every Channel control a file descriptor
class Channel : base::noncopyable {
public:
    typedef std::function<void()> EventCallback;

    Channel();
    Channel(EventLoop* loop , int fd);
    ~Channel();

    // according to different revents to handle Event
    void handleEvent();

    void setReadCallback(const EventCallback& cb) { this->readCallback_ = cb; }
    void setWriteCallback(const EventCallback& cb) { this->writeCallback_ = cb; }
    void setErrorCallback(const EventCallback& cb) { this->errorCallback_ = cb; }

    int fd() const { return this->fd_; }
    int condition() const {return this->condition_; }
    int events() const { return this->events_; }
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() const { return this->events_ == cNoneEvent; }
    void setCondition(int condition) { this->condition_ = condition; }

    EventLoop* ownerLoop() { return this->loop_; }

    void enableReading() { this->events_ |= cReadEvent; this->update(); }

    // all condition of Channel
    static const int cNew;
    static const int cAdd;
    static const int cDel;

    static std::string eventsToString(int fd , int events);

    void setLoop(EventLoop* loop) { this->loop_ = loop; }
    void setFd(int fd) { this->fd_ = fd; }

private:

    void update();

    static const int cNoneEvent;
    static const int cReadEvent;
    static const int cWriteEvent;

    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;
    // condition of epoller
    int condition_;    

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};

}


#endif