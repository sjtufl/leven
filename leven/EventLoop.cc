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
          threadId_(gettid()),
          timerQueue_(this),
          wakeupfd_(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
          wakeupChl_(this, wakeupfd_)
{
    if (wakeupfd_ == -1)
        SYSFATAL("EventLoop::EventLoop() in ctor, eventfd_ creation failed");
    wakeupChl_.setReadCallBack([this](){handleRead();});
    wakeupChl_.enableRead();

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
    if (!isInLoopThread())
        wakeup();
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
    if (!isInLoopThread() || doingPendingJobs_)
        wakeup();
}

void EventLoop::queueInLoop(leven::Task &&task)
{
    {
        std::lock_guard<std::mutex> guard(mutex_);
        pendingJobs_.push_back(std::move(task));
    }

    // fixme: same as above
    if (!isInLoopThread() || doingPendingJobs_)
        wakeup();
}

Timer* EventLoop::runAt(leven::Timestamp when, leven::TimerCallBack cb) {
    return timerQueue_.addTimerTask(std::move(cb), when, Millisecond::zero());
}

Timer* EventLoop::runAfter(leven::Nanosecond interval, leven::TimerCallBack cb) {
    return runAt(clock::now() + interval, std::move(cb));
}

Timer* EventLoop::runEvery(leven::Nanosecond interval, leven::TimerCallBack cb) {
    return timerQueue_.addTimerTask(std::move(cb), clock::now() + interval, interval);
}

void EventLoop::cancelTimer(leven::Timer *timer) {
    timerQueue_.cancelTimer(timer);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupfd_, &one, sizeof(one));
    if (n != sizeof(one))
        SYSERR("EventLoop::wakeup() should ::write() %lu bytes", sizeof(one));
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
    uint64_t one;
    ssize_t n = ::read(wakeupfd_, &one, sizeof(one));
    if (n != sizeof(one))
        SYSERR("EventLoop::handleRead() should ::read() %lu bytes", sizeof(one));
}

