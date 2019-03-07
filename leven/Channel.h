//
// Created by fl on 3/1/19.
//

#ifndef LEVEN_CHANNEL_H
#define LEVEN_CHANNEL_H

#include <functional>
#include <memory>
#include <sys/epoll.h>

#include <leven/noncopyable.h>

namespace leven
{


class EventLoop;

class Channel : noncopyable  // non-value semantic
{
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    typedef std::function<void()> ReadCallBack;
    typedef std::function<void()> WriteCallBack;
    typedef std::function<void()> ErrorCallBack;
    typedef std::function<void()> CloseCallBack;

    void setReadCallBack(const ReadCallBack& rcb)
    { readCallBack_ = rcb; }

    void setWriteCallBack(const WriteCallBack& wcb)
    { writeCallBack_ = wcb; }

    void setErrorCallBack(const ErrorCallBack& ecb)
    { errorCallBack_ = ecb; }

    void setCloseCallBack(const CloseCallBack& ccb)
    { closeCallBack_ = ccb; }

    void handleEvents(); // core of Channel

    bool polling;

    int fd() const
    { return fd_; }

    bool hasNoneEvents() const
    { return events_ == 0; }

    unsigned events() const {
        return events_;
    }

    void setRevents(unsigned revents) {
        revents_ = revents;
    }

    void enableRead()
    { events_ |= (EPOLLIN | EPOLLPRI); update();}
    void enableWrite()
    { events_ |= EPOLLOUT; update();}
    void disableRead()
    { events_ &= ~(EPOLLIN); update();}
    void disableWrite()
    { events_ &= ~(EPOLLOUT); update(); }
    void disableAll()
    { events_ = 0; update();}

    bool isReading() const { return events_ & EPOLLIN; }
    bool isWriting() const { return events_ & EPOLLOUT;}

private:
    void update();
    void remove();

    void handleEventsWithGuard();

    EventLoop* loop_;
    int fd_;

    unsigned events_;
    unsigned revents_; // active events currently

    bool handlingEvents_;

    ReadCallBack readCallBack_;
    WriteCallBack writeCallBack_;
    ErrorCallBack errorCallBack_;
    CloseCallBack closeCallBack_;

};



}

#endif //LEVEN_CHANNEL_H
