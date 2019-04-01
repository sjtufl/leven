//
// Created by fl on 3/21/19.
//

#include "TcpServer.h"
#include "util/Buffer.h"
#include "EventLoop.h"
#include "TcpConnection.h"

using namespace leven;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &local)
            : loop_(loop),
              acceptor_(loop, local)
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2, _3));
}

void TcpServer::start() {
    acceptor_.listen();
}

void TcpServer::newConnection(int connfd, const leven::InetAddress &local, const leven::InetAddress &peer) {
    loop_->assertInLoopThread();
    auto conn = std::make_shared<TcpConnection>(loop_, connfd, local, peer);
    connections_.insert(conn);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallBack(std::bind(
            &TcpServer::closeConnection, this, _1));
    conn->connectEstablished(); // enable and tie channel
    connectionCallback_(conn);
}

void TcpServer::closeConnection(const leven::TcpConnPtr &conn) {
    loop_->assertInLoopThread();
    size_t ret = connections_.erase(conn);
    assert(ret == 1); (void)ret;
    connectionCallback_(conn);
}

