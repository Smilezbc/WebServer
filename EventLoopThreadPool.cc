#include "EventLoopThreadPool.h"

#include "EventLoop.h"
#include "EventLoopThread.h"

#include <boost/bind.hpp>

using namespace webServer;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
  :baseLoop_(baseLoop),
  numThreads_(0),
  nextLoop_(0),
  started_(false)
{
}
~EventLoopThreadPool::EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    assert(!started_);
    baseLoop_->assertInLoopThred();
    started_=true;
    for(int i=0;i<numThreads;++i)
    {
        EventLoopThread* thread=new EventLoopThread();
        threads.push_back(thread);
        loops.push_back(thread->startLoop());
    }
}
EventLoop* getNextLoop()
{
    baseLoop_.assertInLoopThread();
    EventLoop* loop=baseLoop_;
    if(!loops_.empty())
    {
        loop=loops[nextLoop_];
        ++nextLoop_;
        if(static_cast<int>(nextLoop_)>=loops.size())
        {
            nextLoop_=0;
        }
    }
    return loop;
}