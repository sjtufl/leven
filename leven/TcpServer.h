//
// Created by fl on 3/21/19.
//

#ifndef LEVEN_TCPSERVER_H
#define LEVEN_TCPSERVER_H

#include <leven/Callbacks.h>
#include <leven/Acceptor.h>

#include <unordered_set>

namespace leven
{

class EventLoop;

class TcpServer : noncopyable
{
public:
    TcpServer(EventLoop *loop, const InetAddress &local);

    void setConnectCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

    void start();

private:

    void newConnection(int connfd, const InetAddress& local, const InetAddress& peer);

    void closeConnection(const TcpConnPtr& conn);

    typedef std::unordered_set<TcpConnPtr> ConnectionSet;

    EventLoop *loop_;
    Acceptor acceptor_;
    ConnectionSet connections_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
};



} // namespace leven

#endif //LEVEN_TCPSERVER_H
