//
// Created by fl on 3/1/19.
//

#include <cassert>
#include <leven/EventLoop.h>
#include <leven/Channel.h>

using namespace leven;

Channel::Channel(leven::EventLoop *loop, int fd)
        : polling(false),
          loop_(loop),
          fd_(fd),
          events_(0),
          revents_(0),
          tied_(false),
          handlingEvents_(false)
        {}

Channel::~Channel() {
    assert(!handlingEvents_);
}

void Channel::handleEvents() {
    loop_->assertInLoopThread();
    // fixme: should use weak_ptr to extend Channel's life time
    handleEventsWithGuard();
}

void Channel::handleEventsWithGuard() {
    handlingEvents_ = true;
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallBack_) closeCallBack_();
    }
    if ((revents_ & EPOLLERR)) {
        if (errorCallBack_) errorCallBack_();
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (readCallBack_) readCallBack_();
    }
    if (revents_ & EPOLLOUT) {
        if (writeCallBack_) writeCallBack_();
    }
    handlingEvents_ = false;
}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tiedObjPtr_ = obj;
    tied_ = true;
}

void Channel::remove() {
    assert(polling);
    loop_->removeChannel(this);
}
