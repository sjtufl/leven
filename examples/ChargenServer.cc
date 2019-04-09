//
// Created by fl on 4/9/19.
//
#include <leven/EventLoop.h>
#include <leven/TcpConnection.h>
#include <leven/TcpServerThreaded.h>
#include <leven/util/Logger.h>

using namespace leven;
using namespace std::placeholders;

class ChargenServer
{
public:
    ChargenServer(EventLoop* loop, const InetAddress& addr)
            : server_(loop, addr),
              transferred_(0)
    {
        server_.setConnectionCallback(std::bind(
                &ChargenServer::onConnection, this, _1));
        server_.setWriteCompleteCallback(std::bind(
                &ChargenServer::onWriteComplete, this, _1));

        std::string line;
        for (int i = 33; i < 127; ++i)
            line.push_back(char(i));
        line += line;

        for (size_t i = 0; i < 127-33; ++i)
            message_ += line.substr(i, 72) + '\n';
        transferred_ += message_.size();

    }

    void start() {
        server_.start();
    }

    void onConnection(const TcpConnPtr& conn) {
        INFO("Connection %s is [%s]", conn->name().c_str(), conn->connected() ? "up" : "down");
        if (conn->connected()) {
            conn->send("Leven chargen server\n");
            conn->send(message_);
        }
        else {
            INFO("%ld bytes transferred", transferred_);
        }
    }

    void onWriteComplete(const TcpConnPtr& conn) {
        conn->send(message_);
        transferred_ += message_.size();
    }

private:
    TcpServerThreaded server_;
    uint64_t transferred_;
    std::string message_;
};

int main()
{
    EventLoop loop;
    InetAddress addr(10011);
    ChargenServer server(&loop, addr);
    server.start();
    loop.loop();
}

