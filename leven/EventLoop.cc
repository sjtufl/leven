//
// Created by fl on 3/1/19.
//

#include <cassert>
#include <unistd.h>  // syscall()
#include <syscall.h> // gettid
#include <sys/eventfd.h>
#include <sys/types.h> // pid_t

#include <leven/EventLoop.h>
#include <leven/util/Logger.h>
#include <leven/Channel.h>

using namespace leven;

namespace
{

__thread EventLoop* t_eventloop = nullptr;  // thread local variable, for

}


EventLoop::EventLoop()
        : epoller_(this),
          doingPendingJobs_(false),
          quit_(false),
          threadId_(gettid())
{
    assert(t_eventloop == nullptr);
    t_eventloop = this;
}

EventLoop::~EventLoop() {
    assert(t_eventloop == this);
    t_eventloop = nullptr;
}

void EventLoop::loop()
{
    assertInLoopThread();
    TRACE("EventLoop %p is polling", this);
    quit_ = false;
    while (!quit_) {
        activeChnls_.clear();
        epoller_.Poll(activeChnls_);
        for (auto chnl : activeChnls_)
            chnl->handleEvents();
        doPendingJobs();
    }
    TRACE("EventLoop %p quit!", this);
}

void EventLoop::quit()
{
    assert(!quit_);
    quit_ = true;
    // fixme: wakeup?
}

void EventLoop::runInLoop(const leven::Task &task)
{
    if (!isInLoopThread())
        task();
    else
        queueInLoop(task);
}

void EventLoop::runInLoop(Task&& task)
{
    if (isInLoopThread())
        task();
    else
        queueInLoop(std::move(task));
}


void EventLoop::queueInLoop(const leven::Task &task)
{
    // append task to Task queue.
    {
        std::lock_guard<std::mutex> guard(mutex_);
        pendingJobs_.push_back(task);
    }

    // fixme: if doing pending jobs or not in loop thread, wakeup?
}

void EventLoop::queueInLoop(leven::Task &&task)
{
    {
        std::lock_guard<std::mutex> guard(mutex_);
        pendingJobs_.push_back(std::move(task));
    }

    // fixme: same as above
}


void EventLoop::updateChannel(Channel* channel)
{
    assertInLoopThread();
    epoller_.updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assertInLoopThread();
    channel->disableAll();
}


void EventLoop::doPendingJobs() {
    assertInLoopThread();
    std::vector<Task> jobs;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        // shorten critical section by swapping tasks to local stack var. <-> jobs.
        jobs.swap(pendingJobs_);
    }
    doingPendingJobs_ = true;
    for (Task& task : jobs) {
        task();
    }
    doingPendingJobs_ = false;
}

void EventLoop::handleRead()
{

}

