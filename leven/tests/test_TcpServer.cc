//
// Created by fl on 4/8/19.
//
#include <leven/util/Logger.h>
#include <leven/EventLoop.h>
#include <leven/TcpConnection.h>
#include <leven/TcpServer.h>

#include <map>

using namespace leven;
using namespace std::placeholders;

class Echo
{
public:
    Echo(EventLoop* loop, const InetAddress& addr, size_t numThread = 1, Nanosecond timeout = 5s)
            : loop_(loop),
              server_(loop, addr),
//            numThread_(numThread),
              timeout_(timeout),
              timer_(loop_->runEvery(timeout_, [this](){onTimeout();}))
    {
        server_.setConnectCallback(std::bind(
                &Echo::onConnection, this, _1));
        server_.setMessageCallback(std::bind(
                &Echo::onMessage, this, _1, _2));
//        server_.setWriteCompleteCallback(std::bind(
//                &Echo::onWriteComplete, this, _1));
    }

    ~Echo()
    { loop_->cancelTimer(timer_); }

    void start()
    {
        server_.start();
    }

    void onConnection(const TcpConnPtr& conn) {
        INFO("connection %s is [%s]",
             conn->name().c_str(),
             conn->connected() ? "up":"down");

        if (conn->connected()) {
            expireAfter(conn, timeout_);
        }
        else connections_.erase(conn);
    }

    void onMessage(const TcpConnPtr& conn, Buffer& buffer)
    {
        TRACE("connection %s recv %lu bytes",
              conn->name().c_str(),
              buffer.readableBytes());

        // send will retrieve the buffer
        conn->send(buffer);
        expireAfter(conn, timeout_);
    }

private:
    void expireAfter(const TcpConnPtr& conn, Nanosecond interval)
    {
        connections_[conn] = clock::nowAfter(interval);
    }

    void onTimeout()
    {
        for (auto it = connections_.begin(); it != connections_.end(); ) {
            if (it->second <= clock::now()) {
                INFO("connection timeout force close");
                it->first->forceClose();
                it = connections_.erase(it);
            }
            else it++;
        }
    }

private:
    EventLoop* loop_;
    TcpServer server_;
//    const size_t numThread_;
    const Nanosecond timeout_;
    Timer* timer_;
    typedef std::map<TcpConnPtr, Timestamp> ConnectionList;
    ConnectionList connections_;
};


int main()
{
    setLogLevel(LOG_LEVEL_TRACE);
    EventLoop loop;
    InetAddress addr(9877);
    Echo server(&loop, addr, 1, 5s);
    server.start();

    loop.runAfter(10s, [&](){
        int countdown = 5;
        INFO("server quit after %d second...", countdown);
        loop.runEvery(1s, [&, countdown]() mutable {
            INFO("server quit after %d second...", --countdown);
            if (countdown == 0)
                loop.quit();
        });
    });

    loop.loop();
}
