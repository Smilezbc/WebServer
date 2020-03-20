#ifndef WEBSERVER_TIMEQUEUE_H
#define WEBSERVER_TIMEQUEUE_H

#include "Callbacks.h"
#include "Channel.h"

#include<boost/noncopyable.hpp>
#include<set>
#include<vector>

namespace webServer
{

class TimerId;
class Timestamp;
class Timer;
class EventLoop;

class TimeQueue : boost::noncopyable
{
  public:
    typedef std::pair<Timestamp,Timer*> Entry;
    typedef std::set<Entry> TimerSet;
    typedef std::pair<Timer*,int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    TimeQueue(EventLoop* loop);
    ~TimeQueue();

    void addTimer(TimerCallback& cb,Timestamp when,double interval);
    void cancel(TimerId timerId);
  private:
    void handleRead();
    bool insert(Timer* timer);
    std::vector<Entry> getExpired(Timestamp now);
    void reset(std::vector<Entry>& expired,Timestamp now);
    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);

    EventLoop* loop_;

    int timerfd_;
    Channel timerfdChannel_;

    TimerSet timers_;
    ActiveTimerSet activeTimers_;
    ActiveTimerSet cancelTimers_;
    bool callingExpiredTimers_;
};
  
}
#endif