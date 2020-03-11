#include "TimeQueue.h"
#include "logging/Logger.h"
#include "TimeStamp.h"
#include "TimerId.h"
namespace webServer
{
namespace detail
{
int createTimerFd()
{
         //CLOCK_MONOTONIC 是单调时间，即从某个时间点开始到现在过去的时间，用户不能修改这个时间
    int tfd=::timerfd_create(CLOCK_MONOTONIC,
                             TFD_NONBLOCK,TFD_CLOEXEC);
    if(tfd<0)
    {
        LOG_SYSFATAL<<"failed in timefd()";
    }
    return tfd;
}

void readTimerfd(int timerfd,TimeStamp now)
{
    uint64_t one;
    ssize_t n=read(timerfd,one,sizeof one);
    LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
    if(n!=sizeof one)
    {
        LOG_ERROR << "TimerQueue::handleRead() reads "<< n <<"bytes instead of 8";
    }
}
struct timespec howMuchTimeFromNow(const TimeStamp& timeStamp)
{
    TimeStamp now=TimeStamp::now();
    int64_t microSeconds=timeStamp.microSecondsSinceEpoch()-now.microSecondsSinceEpoch();
    if(microSeconds<100)//
    {
        microSeconds=100; //
    }
    struct timespec tspc;
    tspc.tv_sec=static<time_t>(microSeconds/TimeStamp::kMicroSecondsPerSecond);
    //tspc.tv_msec=microSeconds%TimeStamp::kMicroSecondsPerSecond;
    tspc.tv_nsec=static<long>(microSeconds%TimeStamp::kMicroSecondsPerSecond*1000);
    return tspc;
}
void resetTimerfd(int timefd,TimeStamp timeStamp)
{
    struct itimespec oldValue;
    struct itimespec newValue;
    bzero(oldValue,sizeof oldValue);
    bzero(newValue,sizeof newValue);
    newValue.it_value=howMuchTimeFromNow(timeStamp);
    int res=::timefd_settime(timefd,&newValue,&oldValue);
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
    timerfdChannel_.setReadCallback(boost::bind(TimeQueue::handleRead,this);
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
    TimeStamp now=TimeStamp::now();
    readTimerfd(timerfd_,now);
    vector<Entry> expired=getExpired(now);

    callingExpiredTimers_=true;
    cancelTimers_.clear();
    for(Entry& entry:expired)
    {
        entry.second->run();
    }
    callingExpiredTimers_=false;
    reset(expired, now);
}
std::vector<TimeQueue::Entry> TimeQueue::getExpired(TimeStamp now)
{
    std::vector<Entry> expired;
    Entry sentry=pair<TimeStamp,reinterpret_cast<Timer*>>(now,UINTPTR_MAX);
    TimerSet::iterator it = timers_.low_bound(sentry);
    std::copy(timers_.begin(),it,back_inserter(expired));

    timers_.erase(timers_.begin(),it);
    for(Entry &entry:expired)
    {
        ActiveTimer timer(entry.second,entry.second->sequence());
        activeTimers_.erase(timer);
        assert(n == 1); (void)n;
    }
    assert(activeTimers_.size()==timers_.size());
    return expired;
}
void TimeQueue::reset(vector<Entry>& expired,TimeStamp now)
{
    for(Entry& entry:expired)
    {
        ActiveTimer timer(entry->second,entry->second->sequence());
        if(entry->second->repeat() && cancelTimers.find(timer)==cancelTimers.end())
        {
            entry->second->restart(now);
            insert(entry->second);
        }
        else
        {
            delete entry->second;
        }
    }
}
bool TimeQueue::insert(Timer* timer)
{
    loop_->assertInLoopThread();
    assert(timers_.size()==activeTimers_.size());
    bool earliestChanged=false;
    TimeStamp when=timer.expired();
    TimerSet::iterator it=timers_.begin();
    if(it==timers.end() || when < it->fisrt)
    {
        earliestChanged=true;
    }

    {
        pair<TimerSet::iterator,bool> result=timers_.insert(Entry(when,timer));
        assert(result.second);(void)result;
    }
    {
        pair<ActiveTimerSet::iterator,bool> result=activeTimers.insert(ActiveTimer(timer,timer.sequence()));
        assert(result.second); void(result);
    }
    
    assert(timers_.size()==activeTimers_.size());
    return earliestChanged;
}

void TimeQueue::addTimer(TimerCallback& cb,TimeStamp when,double interval)
{
    Timer* timer=new Timer(cb,when,interval);
    loop_->runInLoop(boost::bind(TimeQueue::addTimerInLoop,this,timer));
}
void TimeQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged=insert(timer);
    if(earliestChanged)
    {
        resetTimerfd(timefd_,timer->expired());
    }
}
void TimeQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(boost::bind(TimeQueue::cancelInLoop,this,timerId));
}
void TimeQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(activeTimers_.size()==timers_.size());
    ActiveTimer activeTimer(TimerId.timer_,TimerId.sequence_);
    ActiveTimerSet::iterator it=activeTimers_.find(activeTimer);
    if(it!=activeTimers_.end())
    {
        activeTimers_.earse(it);
        timers_.erase(Entry(timerId.timer_.expired(),timerId.timer_));
        delete it->first;
    }
    else if(callingExpiredTimers_)
    {
        cancelTimers_.insert(activeTimer);
    }
    assert(activeTimers_.size()==timers_.size());
}