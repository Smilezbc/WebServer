#include "Timer.h"
#include "Timestamp.h"
using namespace webServer;


Timer::Timer(TimerCallback cb,Timestamp expiration,double interval)
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
void Timer::restart(Timestamp expiration)
{
    if(repeat_)
    {
        expiration_=addTime(expiration,interval_);
    }
    else
    {
        expiration_=Timestamp::invalid();
    }
}
void Timer::run()
{
    cb_();
}