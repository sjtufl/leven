//
// Created by fl on 10/15/18.
//

#ifndef LEVEN_ACCEPTOR_H
#define LEVEN_ACCEPTOR_H

#include <leven/noncopyable.h>
#include <leven/InetAddress.h>
#include <leven/Channel.h>
#include <leven/Callbacks.h>

namespace leven
{

class Acceptor: noncopyable
{
public:
    Acceptor(EventLoop *loop, const InetAddress& local);
    ~Acceptor();

    bool listning() const {
        return listening_;
    }

    void listen();

    void setNewConnectionCallback(const NewConnectionCallBack& cb)
    {
        newConnectionCallBack_ = cb;
    }

private:
    void handleRead();

    bool listening_;
    EventLoop *loop_;
    const int acceptFd_;
    Channel acceptChannel_;
    InetAddress local_;
    NewConnectionCallBack newConnectionCallBack_;
};


} // namespace leven


#endif //LEVEN_ACCEPTOR_H
