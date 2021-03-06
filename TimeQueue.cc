#include "TimeQueue.h"
#include "logging/Logger.h"
#include "Timestamp.h"
#include "TimerId.h"
#include "EventLoop.h"
#include "Timer.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include<functional>
#include<vector>
#include<assert.h>

namespace webServer
{
namespace detail
{
int createTimerFd()
{
         //CLOCK_MONOTONIC 是单调时间，即从某个时间点开始到现在过去的时间，用户不能修改这个时间
    int tfd=::timerfd_create(CLOCK_MONOTONIC,
                             TFD_NONBLOCK | TFD_CLOEXEC);
    if(tfd<0)
    {
        LOG_SYSFATAL<<"failed in timefd()";
    }
    return tfd;
}

void readTimerfd(int timerfd,Timestamp now)
{
    uint64_t one;
    ssize_t n=read(timerfd,&one,sizeof one);
    LOG_TRACE << "TimerQueue::handleRead() " << n << " at " << now.toString();
    if(n!=sizeof one)
    {
        LOG_ERROR << "TimerQueue::handleRead() reads "<< n <<"bytes instead of 8";
    }
}
struct timespec howMuchTimeFromNow(const Timestamp& timestamp)
{
    Timestamp now=Timestamp::now();
    int64_t microSeconds=timestamp.microSecondsSinceEpoch()-now.microSecondsSinceEpoch();
    if(microSeconds<100)//
    {
        microSeconds=100; //
    }
    struct timespec tspc;
    tspc.tv_sec=static_cast<time_t>(microSeconds/Timestamp::kMicroSecondsPerSecond);
    tspc.tv_nsec=static_cast<long>(microSeconds%Timestamp::kMicroSecondsPerSecond*1000);
    return tspc;
}
void resetTimerfd(int timefd,Timestamp timestamp)
{
    struct itimerspec oldValue;
    struct itimerspec newValue;
    bzero(&oldValue,sizeof oldValue);
    bzero(&newValue,sizeof newValue);
    newValue.it_value=howMuchTimeFromNow(timestamp);
    int res=::timerfd_settime(timefd,0,&newValue,&oldValue);
    if(res<0)
    {
        LOG_SYSERR<<"failed in timefd_settime";
    }
}

}
}

using namespace webServer;
using namespace webServer::detail;


TimeQueue::TimeQueue(EventLoop* loop)
  :loop_(loop),
  timerfd_(createTimerFd()),
  timerfdChannel_(loop_,timerfd_),
  callingExpiredTimers_(false)
{
    timerfdChannel_.setReadCallback(std::bind(TimeQueue::handleRead,this));
    timerfdChannel_.enableRead();
}

TimeQueue::~TimeQueue()
{
    close(timerfd_);
    //要析构所有的Timers_
    for(Entry entry:timers_)
    {
        delete entry.second;
    }
}

void TimeQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now=Timestamp::now();
    readTimerfd(timerfd_,now);
    std::vector<Entry> expired=getExpired(now);

    callingExpiredTimers_=true;
    cancelTimers_.clear();
    for(Entry& entry:expired)
    {
        entry.second->run();
    }
    callingExpiredTimers_=false;
    reset(expired, now);
}
std::vector<TimeQueue::Entry> TimeQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry=std::pair<Timestamp,Timer*>(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerSet::iterator it = timers_.lower_bound(sentry);
    std::copy(timers_.begin(),it,back_inserter(expired));

    timers_.erase(timers_.begin(),it);
    for(Entry &entry:expired)
    {
        ActiveTimer timer(entry.second,entry.second->sequence());
        int n=activeTimers_.erase(timer);
        assert(n == 1); (void)n;
    }
    assert(activeTimers_.size()==timers_.size());
    return expired;
}
void TimeQueue::reset(std::vector<Entry>& expired,Timestamp now)
{
    
    for(Entry& entry:expired)
    {
        ActiveTimer timer(entry.second,entry.second->sequence());
        if(entry.second->repeat() && cancelTimers_.find(timer)==cancelTimers_.end())
        {
            entry.second->restart(now);
            insert(entry.second);
        }
        else
        {
            delete entry.second;
        }
    }
    Timestamp newExpired;
    if(!timers_.empty())
    {
        newExpired=timers_.begin()->second->expiration();
    }
    if(newExpired.valid())
    {
        resetTimerfd(timerfd_,newExpired);
    }
}

bool TimeQueue::insert(Timer* timer)
{
    loop_->assertInLoopThread();
    assert(timers_.size()==activeTimers_.size());
    bool earliestChanged=false;
    Timestamp when=timer->expiration();
    TimerSet::iterator it=timers_.begin();
    if(it==timers_.end() || when < it->first)
    {
        earliestChanged=true;
    }

    {
        std::pair<TimerSet::iterator,bool> result=timers_.insert(Entry(when,timer));
        assert(result.second);(void)result;
    }
    {
        std::pair<ActiveTimerSet::iterator,bool> result=activeTimers_.insert(ActiveTimer(timer,timer->sequence()));
        assert(result.second); 
    }
    
    assert(timers_.size()==activeTimers_.size());
    return earliestChanged;
}


void TimeQueue::addTimer(TimerCallback& cb,Timestamp when,double interval)
{
    Timer* timer=new Timer(cb,when,interval);
    loop_->runInLoop(std::bind(TimeQueue::addTimerInLoop,this,timer));
}

void TimeQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged=insert(timer);
    if(earliestChanged)
    {
        resetTimerfd(timerfd_,timer->expiration());
    }
}
void TimeQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(std::bind(TimeQueue::cancelInLoop,this,timerId));
}
void TimeQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(activeTimers_.size()==timers_.size());
    ActiveTimer activeTimer(timerId.timer_,timerId.sequence_);
    ActiveTimerSet::iterator it=activeTimers_.find(activeTimer);
    if(it!=activeTimers_.end())
    {
        activeTimers_.erase(it);
        timers_.erase(Entry(timerId.timer_->expiration(),timerId.timer_));
        delete it->first;
    }
    else if(callingExpiredTimers_)
    {
        cancelTimers_.insert(activeTimer);
    }
    assert(activeTimers_.size()==timers_.size());
}