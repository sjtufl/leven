//
// Created by fl on 3/7/19.
//

#ifndef LEVEN_EPOLLER_H
#define LEVEN_EPOLLER_H

#include <vector>
#include <leven/noncopyable.h>

namespace leven
{

class EventLoop;
class Channel;

class Epoller : noncopyable
{
public:
    explicit Epoller(EventLoop* loop);
    ~Epoller();

    typedef std::vector<Channel*> ChannelList;

    void Poll(ChannelList& activeChannels);
    // must be called in loop thread.
    void updateChannel(Channel* channel);

private:
    void updateChannel(int op, Channel* channel);
    EventLoop* loop_;
    std::vector<struct epoll_event> events_;
    int pollfd_; //

};


}

#endif //LEVEN_EPOLLER_H
