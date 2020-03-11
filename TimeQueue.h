#ifndef WEBSERVER_TIMEQUEUE_H
#define WEBSERVER_TIMEQUEUE_H

#include<boost/noncopyable.hpp>
#include "Callbacks.h"
namespace webServer
{
class TimerId;
class TimeStamp;
clss Timer;

class TimeQueue : boost::noncopyable
{
  public:
    typedef pair<TimeStamp,Timer*> Entry;
    typedef set<Entry> TimerSet;
    typedef pair<Timer*,int64_t> ActiveTimer;
    typedef set<ActiveTimer> ActiveTimerSet;
    TimeQueue(EventLoop* loop);
    ~TimeQueue();

    void addTimer(TimerCallback& cb,TimeStamp when,double interval);
    void cancel(TimerId timerId);
  private:
    void handRead();
    bool insert(Timer* timer);
    EventLoop* loop_;

    timerfd timefd_;
    Channel timerfdChannel_;

    TimerSet timers_;
    ActiveTimerSet activeTimers_;
    ActiveTimerSet cancelTimers_;
    bool callingExpiredTimers_;
};
  
}
#endif