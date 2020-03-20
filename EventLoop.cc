#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "Timer.h"
#include "TimeQueue.h"
#include "TimerId.h"
#include <algorithm>
#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include<assert.h>

namespace 
{
    
    __thread EventLoop* t_loopInThisThread=nullptr;
    const int kPollWaitTimeMs = 10000;

    int CreateEventfd()
    {
        int fd_=::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if(fd_<0)
        {
            LOG_FATAL << "::eventfd() failed!";
        }
        return fd_;
    }

class IgnoreSigPipe
{
 public:
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE, SIG_IGN);
  }
};
IgnoreSigPipe initObj;

}


using namespace webServer;

EventLoop::EventLoop()
  :threadId_(CurrentThread::tid()),
  looping_(false),
  quit_(false),
  callingPendingFunctors_(false),
  poller_(new Poller(this)),
  timeQueue_(new TimeQueue(this)),
  wakeupFd_(CreateEventfd()),
  wakeupChannel_(new Channel(this,wakeupFd_))
{
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_; 
    if(t_loopInThisThread)
    {
        LOG_FATAL<<"Another EventLoop "<< t_loopInThisThread << "exists in this thread " << threadId_; 
    }
    else
    {
        t_loopInThisThread=this;
    }
    wakeupChannel_->setReadCallback(std::bind(EventLoop::handRead,this));
    wakeupChannel_->enableRead();
}
EventLoop::~EventLoop()
{
    assert(!looping_);
    ::close(wakeupFd_);
    t_loopInThisThread=nullptr;//要在eventLoop析构时，置为0
}

void EventLoop::loop()
{
    assert(!looping_);//
    assertInLoopThread();
    looping_=true;
    quit_=false;
    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollWaitTimeMs,activeChannels_);

        for(ChannelList::iterator it=activeChannels_.begin();it!=activeChannels_.end();++it)
        {
            (*it)->handleEvent(pollReturnTime_);
        }
        doPendingFunctors();
    }
    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_=false;
}
void EventLoop::quit()
{
    quit_=true;
    if(!isInLoopThread())//
    {//
        wakeup();//
    }//
}


//**********************关于pendingFunctors部分*********************

//用来唤醒poll，以便执行pendingFunctors
void EventLoop::wakeup()
{
    uint64_t one;
    ssize_t n=::write(wakeupFd_,&one,sizeof one);
    if(n<sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeup() write "<<n<<" bytes instead of 8";
    }
}
void EventLoop::handRead()
{
    uint64_t one; //int64_t one;
    ssize_t n=::read(wakeupFd_,&one,sizeof one);//int n=::read(wakeupFd_,&one,sizeof one);
    if(n != sizeof one)
    {
        LOG_ERROR<< "EventLoop::handRead() reads " <<n<<" bytes instead of 8";
    }
    //quit_=true;
}
void EventLoop::runInLoop(Functor functor)
{
    if(isInLoopThread())
    {
        functor();
    }
    else
    {
        queueInLoop(functor);
    }
}
void EventLoop::queueInLoop(Functor functor)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(functor);
    }
    if(!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

void EventLoop::doPendingFunctors()
{
    callingPendingFunctors_=true;
    FunctorList pendingFunctors;
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors.swap(pendingFunctors_);
    }
    for(FunctorList::iterator it=pendingFunctors.begin();it!=pendingFunctors.end();++it)
    {
        (*it)();
    }
    callingPendingFunctors_=false;
}

//*************************关于监听描述符部分*************************************
void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->loop()==this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->loop()==this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}

//*****************************************关于定时器的部分**************************

void EventLoop::runAt(TimerCallback cb,Timestamp when)
{
    timeQueue_->addTimer(cb,when,0.0);//必须保证在loop线程中添加
}
void EventLoop::runAfter(TimerCallback cb,double delay)
{
    Timestamp when = addTime(Timestamp::now(),delay);
    runAt(cb,when);
}
void EventLoop::runEvery(TimerCallback cb,double interval) //interval是秒数
{
    Timestamp when = addTime(Timestamp::now(),interval);
    timeQueue_->addTimer(cb,when,interval);
}
//Timer的地址是不能用来区分不同的Timer的
//因为同一个地址可能已经创建销毁过Timer
void EventLoop::cancel(TimerId timerId)
{
    timeQueue_->cancel(timerId);
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this 
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " <<  CurrentThread::tid();
}
