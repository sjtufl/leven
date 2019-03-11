//
// Created by fl on 3/1/19.
//
#ifndef LEVEN_EVENTLOOP_H
#define LEVEN_EVENTLOOP_H

#include <syscall.h> //
#include <unistd.h> // ::syscall
#include <sys/types.h> // pid_t
#include <cassert>

#include <atomic>

#include <leven/noncopyable.h>
#include <leven/Epoller.h>
#include <leven/Callbacks.h>
#include <mutex>


namespace
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

} // namespace

namespace leven
{

class Channel;

class EventLoop : noncopyable
{
public:
    EventLoop ();
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(const Task& task);
    void runInLoop(Task&& task);
    void queueInLoop(const Task& task);
    void queueInLoop(Task&& task);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread() {
        assert(isInLoopThread());
    }

    void assertNotInLoopThread() {
        assert(!isInLoopThread());
    }

    bool isInLoopThread() const {
        return threadId_ == gettid();  // constant, no need to worry about thread safety
    }

private:
    void handleRead();
    void doPendingJobs();

    Epoller epoller_;
    Epoller::ChannelList activeChnls_;

    std::vector<Task> pendingJobs_;
    std::mutex mutex_;

    bool looping_;          // status
    bool doingPendingJobs_; // status
    std::atomic_bool quit_; // thread safe
    const pid_t threadId_;  // id of loop thread
};


} // namespace leven

#endif //LEVEN_EVENTLOOP_H
