#include <unistd.h>
#include <sys/socket.h>
#include <assert.h>

#include <leven/Connector.h>
#include <leven/Logger.h>
#include <leven/InetAddress.h>

using namespace leven;

int Connector::createSocket() {
    int ret = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (ret == -1)
        SYSFATAL("Connector::socket()");
    return ret;
}

Connector::Connector(const leven::InetAddress &peer)
        : peer_(peer),
          sockfd_(createSocket()),
          connected_(false),
          started_(false)
{
}

Connector::~Connector()
{
    if (!connected_)
        ::close(sockfd_);
}

void Connector::start()
{
    assert(!started_);
    started_ = true;

    int ret = ::connect(sockfd_, peer_.getSockaddr(), peer_.getSocklen());
    if (ret == -1) {
//        if (errno != EINPROGRESS)
            //
    } else {
        // handle
    }
}
