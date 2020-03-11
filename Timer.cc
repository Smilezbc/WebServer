#include "Timer.h"
#include "TimeStamp.h"
using namespace webServer;


Timer::Timer(TimerCallback cb,TimeStamp expiration,double interval)
  :cb_(cb),
  expiration_(expiration),
  interval_(interval),
  repeat_(interval>0.0),
  sequence_(s_numCreated_.incrementAndGet())
{
}
Timer::~Timer()
{
    
}
void Timer::restart(TimeStamp expiration)
{
    if(repeat_)
    {
        expiration_=addTime(expiration,interval_);
    }
    else
    {
        expiration_=TimeStamp::invalid();
    }
}
void Timer::run()
{
    cb_();
}