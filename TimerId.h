#ifndef WEBSERVER_TIMERID_H
#define WEBSERVER_TIMERID_H

namespace webServer
{

class Timer;

class TimerId
{
  public:
    TimerId(Timer timer,int64_t sequence)
      :timer_(timer),
      sequence_(sequence)
    {
    }
    Timer timer_;
    int64_t sequence_;
};
}
#endif