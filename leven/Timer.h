//
// Created by fl on 3/7/19.
//

#ifndef LEVEN_TIMER_H
#define LEVEN_TIMER_H

#include <cassert>

#include <leven/Callbacks.h>
#include <leven/Channel.h>
#include <leven/util/Timestamp.h>

namespace leven
{

class Timer : noncopyable
{
public:
    Timer(TimerCallBack cb, Timestamp when, Nanosecond interval)
        : callback_(std::move(cb)),
          when_(when),
          interval_(interval),
          repeat_(interval_ > Nanosecond::zero()),
          canceled_(false)
    {}

    void run() { if (callback_) callback_(); }
    bool repeat() const { return repeat_; }
    bool expired(Timestamp now) const {return now >= when_; }
    Timestamp when() const { return when_; }
    void restart()
    {
        assert(repeat_);
        when_ += interval_;
    }
    void cancel()
    {
        assert(!canceled_);
        canceled_ = true;
    }
    bool canceled() const { return canceled_; }

private:
    TimerCallBack callback_;
    Timestamp when_;
    const Nanosecond interval_;
    bool repeat_;
    bool canceled_;

};

}

#endif //LEVEN_TIMER_H
