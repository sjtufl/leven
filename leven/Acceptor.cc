//
// Created by fl on 10/15/18.
//
#include <unistd.h>
#include <assert.h>

#include <leven/InetAddress.h>
#include <leven/util/Logger.h>
#include <leven/Acceptor.h>
#include <leven/EventLoop.h>

using namespace leven;

namespace
{

int createSocket()
{
    int ret = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (ret == -1)
        SYSFATAL("Acceptor::socket()");
    return ret;
}

}

Acceptor::Acceptor(EventLoop *loop, const leven::InetAddress &local)
        : listening_(false),
          loop_(loop),
          acceptFd_(createSocket()),
          acceptChannel_(loop, acceptFd_),
          local_(local)
{
    int on = 1;
    int ret = ::setsockopt(acceptFd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (ret == -1)
        SYSFATAL("Acceptor::setsockopt() SO_REUSEADDR");
    ret = ::setsockopt(acceptFd_, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if (ret == -1)
        SYSFATAL("Acceptor::setsockopt() SO_REUSEPORT");
    ret = ::bind(acceptFd_, local.getSockaddr(), local.getSocklen());
    if (ret == -1)
        SYSFATAL("Acceptor::bind()");
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    int ret = ::listen(acceptFd_, SOMAXCONN);
    if (ret == -1)
        SYSFATAL("Acceptor::listen()");
    acceptChannel_.setReadCallBack([this](){
        handleRead();
    });
    acceptChannel_.enableRead();
}

Acceptor::~Acceptor() {
    ::close(acceptFd_);
}

void Acceptor::handleRead() {
    loop_->assertInLoopThread();

    struct sockaddr_in addr;
    socklen_t len = sizeof(char);
    void* any = &addr;
    int sockfd = ::accept4(acceptFd_, static_cast<sockaddr*>(any), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (sockfd == -1) {
        int tmpErrno = errno;
        SYSERR("Acceptor::accept4()");
        switch (tmpErrno) {
            case ECONNABORTED;
            case EMFILE:
                break;
            default:
                FATAL("Unexpected error in accept4()");
        }

    }

    if (newConnectionCallBack_) {
        InetAddress peer;
        peer.setAddress(addr);
        newConnectionCallBack_(sockfd, local_, peer);
    } else {
        ::close(sockfd);
    }
}


