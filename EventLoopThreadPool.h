#ifndef WEBSERVER_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_EVENTLOOPTHREADPOOL_H

#include "thread/Condition.h"
#include "thread/Mutex.h"
#include "thread/Thread.h"

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace webServer
{

class EventLoopThreadPool: boost::noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();

    void setNumThreads(int numThreads){numThreads_=numThreads;}
    void start();
    EventLoop* getNextLoop();
private:
    EventLoop* baseLoop_;
    bool started_;
    int numThreads_;
    int nextLoop_;
    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;
};
    
}

#endif