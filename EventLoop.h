#ifndef GRT_EVENTLOOP_H
#define GRT_EVENTLOOP_H

#include <thread>
#include <vector>
#include <memory>

#include "./Base/Noncopyable.h"

namespace grt
{
class Channel;
class Epoller;

class EventLoop : base::noncopyable {
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    // used to assert , make sure in this loop's thread
    void assertInLoopThread();

    bool isInLoopThread() const { return this->threadId_ == std::this_thread::get_id(); }

    void updateChannel(Channel* channel);
private:

    void abortNotInLoopThread();

    typedef std::vector<Channel*> ChannelList;

    bool is_loop_;      
    bool is_quit_;
    // this loop's thread id
    const std::thread::id threadId_;

    std::unique_ptr<Epoller> epoller_;
    ChannelList activeChannels_;
};
}


#endif