#ifndef WEBSERVER_EVENTLOOPTHREAD_H
#define WEBSERVER_EVENTLOOPTHREAD_H

#include "thread/Condition.h"
#include "thread/Mutex.h"
#include "thread/Thread.h"

#include <boost/noncopyable.hpp>

namespace webServer
{

class EventLoopThread
{
public:
    EventLoopThread(/* args */);
    ~EventLoopThread();

    EventLoop* startLoop();

private:

    void threadFunc();
    
    bool exiting_;
    MutexLock mutex_;
    Condition cond_;
    EventLoop* loop_;
    Thread thread_;
};
    
}
#endif