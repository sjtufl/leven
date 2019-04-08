//
// Created by fl on 4/7/19.
//

#ifndef LEVEN_TCPSERVERTHREADED_H
#define LEVEN_TCPSERVERTHREADED_H

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <vector>

#include <leven/Callbacks.h>
#include <leven/noncopyable.h>
#include <leven/InetAddress.h>

namespace leven
{

class EventLoop;
class InetAddress;
class TcpServer;

class TcpServerThreaded : noncopyable
{
public:
    TcpServerThreaded(EventLoop* loop, const InetAddress& local);
    ~TcpServerThreaded();

    void setNumThread(size_t n);

    void start();

    void setThreadInitCallback(const ThreadInitCallback& cb) {
        threadInitCallback_ = cb;
    }
    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

private:

    typedef std::unique_ptr<TcpServer> TcpServerPtr;
    typedef std::unique_ptr<std::thread> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadPtrList;
    typedef std::vector<EventLoop*> EventLoopList;

    void startInLoop();
    void runInThread(size_t index);

    EventLoop* baseLoop_;
    size_t numThreads_;
    std::atomic_bool started_;
    TcpServerPtr baseServer_;
    ThreadPtrList threads_;
    EventLoopList eventLoops_;
    InetAddress local_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback threadInitCallback_;
    ConnectionCallback connectionCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    MessageCallback messageCallback_;

};

} // namespace leven

#endif //LEVEN_TCPSERVERTHREADED_H
