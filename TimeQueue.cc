#include "TimeQueue.h"
#include "logging/Logger.h"

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
  
}
}
using namespace webServer;
using namespace webServer::detail;
TimeQueue::TimeQueue(EventLoop* loop)
  :loop_(loop),
  timerfd_(createTimerFd()),
  timerfdChannel_(loop_,timerfd_)
{
}
TimeQueue::~TimeQueue()
{
    close(timerfd_);
}