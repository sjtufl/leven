//
// Created by fl on 3/7/19.
//

#include <unistd.h>
#include <sys/epoll.h>
#include <cassert>

#include <leven/EventLoop.h>
#include <leven/Epoller.h>
#include <leven/util/Logger.h>
#include <cerrno>

using namespace leven;

Epoller::Epoller(leven::EventLoop *loop)
        :loop_(loop),
         events_(128),
         pollfd_(::epoll_create1(EPOLL_CLOEXEC))
{
    if (pollfd_ == -1)
        SYSFATAL("Epoller::epoll_create1() return -1");
}

Epoller::~Epoller() {
    ::close(pollfd_);
}

void Epoller::Poll(ChannelList& activeChannels) {
    loop_->assertInLoopThread(); // must in loop thread
    int maxEvents = static_cast<int>(events_.size());
    int nEvents = epoll_wait(pollfd_, events_.data(), maxEvents, -1);
    if (nEvents == -1) {
        if (errno != EINTR) SYSERR("Epoller::epoll_wait() return -1");
    } else if (nEvents > 0) {
        for (int i = 0; i < nEvents; ++i) {
            auto channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->setRevents(events_[i].events);
            activeChannels.push_back(channel);
        }
        if (nEvents == maxEvents)
            events_.resize(2 * events_.size());
    }
}

void Epoller::updateChannel(Channel *channel) {
    loop_->assertInLoopThread();
    int op = 0;
    if (!channel->polling) {
        assert(!channel->hasNoneEvents());
        op = EPOLL_CTL_ADD;
        channel->polling = true;
    } else if (!channel->hasNoneEvents()) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_DEL;
        channel->polling = false;
    }
    updateChannel(op, channel);
}

void Epoller::updateChannel(int op, Channel *channel) {
    struct epoll_event ev;
    ev.events = channel->events();
    ev.data.ptr = channel;
    int ret = ::epoll_ctl(pollfd_, op, channel->fd(), &ev);
    if (ret == -1)
        SYSERR("Epoller::epoll_ctl()");
}
