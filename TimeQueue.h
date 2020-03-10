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
    EventLoop* loop_;

    timerfd timefd_;
    Channel timerfdChannel_;

    TimerSet timerSet_;
    ActiveTimerSet activeTimerSet_;
    ActiveTimerSet cancelTimerSet_;
};
  
}
#endif