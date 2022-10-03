#ifndef GRT_EPOLLER_H
#define GRT_EPOLLER_H
#include "Log/Log.h"
#include "Base/Noncopyable.h"
#include "EventLoop.h"

#include <vector>
#include <map>
#include <sys/epoll.h>

namespace grt
{

class Channel;

// IO Multiplexing with epoll
class Epoller : base::noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;

    Epoller(EventLoop* loop);
    ~Epoller();

    // Epoll the IO events
    // if timeoutMs is the epoll waiting time 
    // if success return the active file descriptor 's channels
    void epoll(int timeoutMs , ChannelList* activeChannels);

    // changes the interested io events
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void update(int operation , Channel* channel);

    void assertInLoopThread() { this->ownerLoop_->assertInLoopThread(); }

    static std::string operationToString(int operation);

private:

    // fill the channelList according to file descriptor epoll returned
    void fillActiveChannels(int numEvents , ChannelList* activeChannels) const;
    typedef std::vector<struct epoll_event> EpollEventList;
    typedef std::map<int , Channel*> ChannelMap;

    EventLoop* ownerLoop_;
    EpollEventList activeevents_;
    ChannelMap channels_;

    int epfd_;
};
}

#endif