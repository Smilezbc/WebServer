#ifndef WEBSERVER_TIMEER_H
#define WEBSERVER_TIMEER_H

#include "Callbacks.h"
#include "Timestamp.h"
#include "thread/Atomic.h"

namespace webServer
{
  
class Timer
{
  public:
    Timer(TimerCallback,Timestamp,double);
    ~Timer();

    void run();
    void restart(Timestamp time);

    Timestamp expiration(){return expiration_;}
    int64_t sequence(){return sequence_;}
    bool repeat(){return repeat_;}

  private:
    TimerCallback cb_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;
    
    static AtomicInt64 s_numCreated_;
    
};
}
#endif