//
// Created by fl on 3/21/19.
//

#ifndef LEVEN_TIMERQUEUE_H
#define LEVEN_TIMERQUEUE_H

#include <memory>
#include <set>

#include <leven/Timer.h>
#include <leven/Channel.h>

namespace leven
{

class TimerQueue: noncopyable {
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    Timer* addTimerTask(TimerCallBack cb, Timestamp ts, Nanosecond interval);
    void cancelTimer(Timer* timer);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);

private:
    EventLoop* loop_;
    const int timerfd_;
    Channel timerchl_;
    TimerList timers_;

};



} // namespace leven

#endif //LEVEN_TIMERQUEUE_H
