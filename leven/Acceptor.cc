//
// Created by fl on 10/15/18.
//
#include <unistd.h>
#include <assert.h>

#include <leven/InetAddress.h>
#include <leven/util/Logger.h>
#include <leven/Acceptor.h>

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

Acceptor::Acceptor(const leven::InetAddress &local)
        : listening_(false),
          acceptFd_(createSocket()),
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
    int ret = ::listen(acceptFd_, SOMAXCONN);
    if (ret == -1)
        SYSFATAL("Acceptor::listen()");
}

Acceptor::~Acceptor() {
    ::close(acceptFd_);
}


