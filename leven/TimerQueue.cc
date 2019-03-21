//
// Created by fl on 3/21/19.
//

#include <sys/timerfd.h>
#include <strings.h> // bzero
#include <unistd.h>
#include <ratio> // std::nano

#include <leven/util/Logger.h>
#include <leven/EventLoop.h>
#include <leven/TimerQueue.h>

using namespace leven;

namespace // timerfd related
{

int timerfdCreate()
{
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (fd == -1)
        SYSFATAL("timerfd_create() failed");
    return fd;
}

void timerfdRead(int fd)
{
    uint64_t val;
    ssize_t n = read(fd, &val, sizeof(val));
    if (n != sizeof(val))
        ERROR("timerfdRead get %ld, not %lu", n, sizeof(val));
}

struct timespec durationFromNow(Timestamp ts)
{
    struct timespec ret;
    Nanosecond ns = ts - clock::now();
    if (ns < 1ms) ns = 1ms;
    ret.tv_sec = static_cast<time_t >(ns.count()) / std::nano::den;
    ret.tv_nsec = ns.count() % std::nano::den;
    return ret;
}

void timerfdSet(int fd, Timestamp ts) {
    struct itimerspec oldtime, newtime;
    bzero(&oldtime, sizeof(itimerspec));
    bzero(&newtime, sizeof(itimerspec));
    newtime.it_value = durationFromNow(ts);

    int ret = timerfd_settime(fd, 0, &newtime, &oldtime);
    if (ret == -1)
        SYSERR("timerfd_settime() failed");
}

}

TimerQueue::TimerQueue(leven::EventLoop *loop)
            : loop_(loop),
              timerfd_(timerfdCreate()),
              timerchl_(loop, timerfd_)
{
    loop_->assertInLoopThread();
    timerchl_.setReadCallBack([this](){handleRead();});
    timerchl_.enableRead();
}

TimerQueue::~TimerQueue() {
    for (auto& p:timers_)
        delete p.second;
    ::close(timerfd_);
}

Timer* TimerQueue::addTimerTask(leven::TimerCallBack cb, leven::Timestamp ts, leven::Nanosecond interval) {
    Timer* timer = new Timer(std::move(cb), ts, interval);
    loop_->runInLoop([=](){
        auto ret = timers_.insert({ts, timer});
        assert(ret.second);

        if (timers_.begin() == ret.first)
            timerfdSet(timerfd_, ts);
    });
    return timer;
}

void TimerQueue::cancelTimer(leven::Timer *timer) {
    loop_->runInLoop([timer, this](){
        timer->cancel();
        timers_.erase({timer->when(), timer});
        delete timer;
    });
}

void TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    timerfdRead(timerfd_);

    Timestamp now(clock::now());
    for (auto& ele: getExpired(now)) {
        Timer* timer = ele.second;
        assert(timer->expired(now));

        if(!timer->canceled())
            timer->run();
        if (!timer->canceled() && timer->repeat()) {
            timer->restart();
            ele.first = timer->when();
            timers_.insert(ele);
        } else
            delete timer;
    }

    if (!timers_.empty())
        timerfdSet(timerfd_, timers_.begin()->first);
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(leven::Timestamp now) {
    Entry entry(now + 1ns, nullptr);
    std::vector<Entry> entries;
    auto end = timers_.lower_bound(entry);
    entries.assign(timers_.begin(), end);
    timers_.erase(timers_.begin(), end);

    return entries;
}
