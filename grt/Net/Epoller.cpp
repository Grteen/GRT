#include "Epoller.h"
#include "Channel.h"

namespace grt
{

const int cEventListInitSize = 16;

Epoller::Epoller(EventLoop* loop) 
    : ownerLoop_(loop) ,
      epfd_(::epoll_create(1)) ,
      activeevents_(cEventListInitSize)
{
    if (epfd_ < 0) {
        LOG(CRIT , "Epoller::Epoller");
    }
}

Epoller::~Epoller() {}

void Epoller::epoll(int timeoutMs , ChannelList* activeChannels) {
    int nfds = ::epoll_wait(this->epfd_ , this->activeevents_.data() , static_cast<int>(this->activeevents_.size()) , timeoutMs);
    // epoll success
    if (nfds > 0) {
        LOG(DEBUG , "epoll get %d events happened" , nfds);
        this->fillActiveChannels(nfds , activeChannels);
    }
    else if (nfds == 0) {
        LOG(DEBUG , "epoll get nothing");
    }
    // epoll error
    else {
        LOG(ERROR , "Epoller::epoll()");
    }
}

void Epoller::fillActiveChannels(int numEvents , ChannelList* activeChannels) const {
    for (int i = 0 ; i < numEvents ; i++) {
        // find the active file descriptor's channel
        Channel* channel = static_cast<Channel*>(this->activeevents_[i].data.ptr);
#ifdef GRT_DEBUG
        auto it = this->channels_.find(channel->fd());
        assert(it != this->channels_.end());
        assert(it->second == channel);
#endif
        // set the revents which is the channel's file descriptor's revents
        channel->set_revents(this->activeevents_[i].events);
        activeChannels->push_back(channel);
    }
}

void Epoller::updateChannel(Channel* channel) {
    this->assertInLoopThread();
    LOG(DEBUG , "fd = %ld events = %ld" , channel->fd() , channel->events());
    // a new channel (file descriptor), add to channels_ (epollevents_)
    if (channel->condition() == Channel::cNew || channel->condition() == Channel::cDel) {
        if (channel->condition() == Channel::cNew) {
            assert(this->channels_.find(channel->fd()) == this->channels_.end());
            this->channels_.insert(std::make_pair(channel->fd() , channel));
        }
        else if (channel->condition() == Channel::cDel) {
            auto res_find = this->channels_.find(channel->fd());
            // this channel must be in channels_
            assert(res_find != this->channels_.end());
            assert((*res_find).second == channel);
        }

        channel->setCondition(Channel::cAdd);
        this->update(EPOLL_CTL_ADD , channel);
    }
    // update a existing one
    else {
        // assert this chanel can be found in channels
        assert(this->channels_.find(channel->fd()) != this->channels_.end());
        assert(this->channels_[channel->fd()] == channel);
        assert(channel->condition() == Channel::cAdd);

        // no event so delete it from epoll
        if (channel->isNoneEvent()) {
            this->update(EPOLL_CTL_DEL , channel);
            channel->setCondition(Channel::cDel);
        }
        else {
            this->update(EPOLL_CTL_MOD , channel);
        }
    }
}

void Epoller::removeChannel(Channel* channel) {
    this->assertInLoopThread();
    auto res = this->channels_.find(channel->fd());
    assert(res != this->channels_.end());
    assert(res->second == channel);
    assert(channel->isNoneEvent());
    size_t n = this->channels_.erase(channel->fd());
    assert(n == 1);
    this->update(EPOLL_CTL_DEL , channel);
}

std::string Epoller::operationToString(int operation) {
    std::string res;
    if (operation == EPOLL_CTL_ADD) {
        res.assign("EPOLL_CTL_ADD");
    }
    else if (operation == EPOLL_CTL_DEL) {
        res.assign("EPOLL_CTL_DEL");
    }
    else if (operation == EPOLL_CTL_MOD) {
        res.assign("EPOLL_CTL_MOD");
    }
    else {
        res.assign("Unknown Operation");
    }

    return res;
}

void Epoller::update(int operation , Channel* channel) {
    struct epoll_event ev;
    ev.events = channel->events();
    ev.data.ptr = channel;
    LOG(DEBUG , "epoll_ctl operation = %s fd = %ld event = %s" , Epoller::operationToString(operation).c_str() ,
        channel->fd() , Channel::eventsToString(channel->fd() , channel->events()).c_str());

    if (epoll_ctl(this->epfd_ , operation , channel->fd() , &ev) < 0) {
        LOG(ERROR , "epoll_ctl operation = %s" , Epoller::operationToString(operation).c_str());
    }
}

}