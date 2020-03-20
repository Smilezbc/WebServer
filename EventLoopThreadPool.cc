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

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start()
{
    assert(!started_);
    baseLoop_->assertInLoopThread();
    started_=true;
    for(int i=0;i<numThreads_;++i)
    {
        EventLoopThread* thread=new EventLoopThread();
        threads_.push_back(thread);
        loops_.push_back(thread->startLoop());
    }
}
EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    EventLoop* loop=baseLoop_;
    if(!loops_.empty())
    {
        loop=loops_[nextLoop_];
        ++nextLoop_;
        if(static_cast<int>(nextLoop_)>=loops_.size())
        {
            nextLoop_=0;
        }
    }
    return loop;
}