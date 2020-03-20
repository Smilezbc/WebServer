#include "EventLoopThread.h"
#include "EventLoop.h"

#include <boost/bind.hpp>

using namespace webServer;

EventLoopThread::EventLoopThread()
  :exiting_(false),
  mutex_(),
  cond_(mutex_),
  loop_(nullptr),
  thread_(boost::bind(EventLoopThread::threadFunc,this))
{
}
EventLoopThread::~EventLoopThread()
{
    exiting_=true;
    loop_->quit();
    thread_.join();
}
void EventLoopThread::threadFunc()
{
    EventLoop eventloop;

    {
        MutexLockGuard lock(mutex_);
        loop_=&eventloop;
        cond_.notify();
    }
    eventloop.loop();
}
EventLoop* EventLoopThread::startLoop()
{
    assert(!thread_.started());//
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        while(loop_==nullptr)
        {
            cond_.wait();
        }
    }
    return loop_;
}