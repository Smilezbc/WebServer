#ifndef WEBSERVER_TIMEER_H
#define WEBSERVER_TIMEER_H

#include "Callbacks.h"
#include "TimeStamp.h"
#include "thread/Atomic.h"

namespace webServer
{
  
class Timer
{
  public:
    Timer(TimerCallback,TimeStamp,double);
    ~Timer();

    void run();
    void restart(TimeStamp time);

    TimeStamp expiration(){return expiration_;}
    int64_t sequence(){return sequence_;}
    bool repeat(){return repeat_;}

  private:
    TimerCallback cb_;
    TimeStamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;
    
    static AtomicInt64 s_numCreated_;
    
};
}
#endif