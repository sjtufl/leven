//
// Created by fl on 4/7/19.
//

#include <leven/util/Logger.h>
#include <leven/TcpConnection.h>
#include <leven/EventLoop.h>
#include <leven/TcpServer.h>
#include <leven/TcpServerThreaded.h>

using namespace leven;

namespace leven
{

void defaultThreadInitCallback(size_t index)
{
    TRACE("EventLoop thread #%lu started", index);
}

void defaultConnectionCallback(const TcpConnPtr& conn)
{
    INFO("connection %s -> %s %s",
         conn->peer().toIpPort().c_str(),
         conn->local().toIpPort().c_str(),
         conn->connected() ? "up" : "down");
}

void defaultMessageCallback(const TcpConnPtr& conn, Buffer& buffer)
{
    TRACE("connection %s -> %s recv %lu bytes",
          conn->peer().toIpPort().c_str(),
          conn->local().toIpPort().c_str(),
          buffer.readableBytes());
    buffer.retrieveAll();
}

}

TcpServerThreaded::TcpServerThreaded(EventLoop *loop, const InetAddress &local)
                    : baseLoop_(loop),
                      numThreads_(1), // single thread by default
                      started_(false),
                      local_(local),
                      threadInitCallback_(defaultThreadInitCallback),
                      connectionCallback_(defaultConnectionCallback),
                      messageCallback_(defaultMessageCallback)
{
    INFO("Create TcpServerThreaded() %s", local.toIpPort().c_str());
}

TcpServerThreaded::~TcpServerThreaded()
{
    for (auto& ele : eventLoops_)
        if (ele != nullptr)
            ele->quit();
    for (auto& thread : threads_)
        thread->join();
    TRACE("~TcpServerThreaded()");
}

void TcpServerThreaded::setNumThread(size_t n) {
    baseLoop_->assertInLoopThread();
    assert(n > 0);
    assert(!started_);
    numThreads_ = n;
    eventLoops_.resize(n);
}

void TcpServerThreaded::start()
{
    if (started_.exchange(true))
        return;
    baseLoop_->runInLoop([=](){
        startInLoop();
    });
}


void TcpServerThreaded::startInLoop()
{
    INFO("TcpServerThreaded::start() %s with %lu EventLoop Thread(s)",
            local_.toIpPort().c_str(), numThreads_);

    baseServer_ = std::make_unique<TcpServer>(baseLoop_, local_);
    baseServer_->setConnectCallback(connectionCallback_);
    baseServer_->setMessageCallback(messageCallback_);
    baseServer_->setWriteCompleteCallback(writeCompleteCallback_);
    threadInitCallback_(0);
    baseServer_->start();

    for (size_t i = 1; i < numThreads_; ++i) {
        auto thread = new std::thread(std::bind(&TcpServerThreaded::runInThread, this, i));
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (eventLoops_[i] == nullptr)
                cond_.wait(lock);
        }
        threads_.emplace_back(thread);
    }
}

void TcpServerThreaded::runInThread(size_t index)
{
    EventLoop loop;
    TcpServer server(&loop, local_);
    server.setConnectCallback(connectionCallback_);
    server.setMessageCallback(messageCallback_);
    server.setWriteCompleteCallback(writeCompleteCallback_);
    {
        std::lock_guard<std::mutex> guard(mutex_);
        eventLoops_[index] = &loop;
        cond_.notify_one();
    }

    threadInitCallback_(index);
    server.start();
    loop.loop();
    eventLoops_[index] = nullptr;
}
