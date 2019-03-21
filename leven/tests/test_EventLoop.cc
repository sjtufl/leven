//
// Created by fl on 3/8/19.
//
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <sys/timerfd.h>
#include <strings.h>

#include <leven/EventLoop.h>

using namespace leven;

EventLoop* ploop;

void timeout()
{
    printf("Timeout!\n");
    ploop->quit();
}

int main()
{
    EventLoop loop;
    ploop = &loop;
    
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd == -1) 
        exit(1);
    Channel channel(&loop, timerfd);
    channel.setReadCallBack(timeout);
    channel.enableRead();
    
    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);
    
    loop.loop();
    
    ::close(timerfd);
}

