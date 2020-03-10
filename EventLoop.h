#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H


#include "logging/Logger.h"
#include "thread/Mutex.h"
#include "thread/Thread.h"

#include<vector>

namespace webServer
{

class Channel;
class Poller;
class TimeQueue;

class EventLoop : boost::noncopyable
{
  public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    
    

    void updateChannel(Channel*);
    void removeChannel(Channel*);

    void runAt(TimerCallBack cb,TimeStamp when);
    void runAfter(TimerCallBack cb,TimeStamp when);
    void runEvery(TimerCallBack cb,double interval);
    void cancel(TimerId timerId);//用Timer的序列号来取消某个计时器；用Timer的地址来取消timer是不可靠的

    void wakeup();
    void runInLoop(Functor& functor);
    void queueInLoop(Functor& functor);

    bool isInLoopThread(){return threadId_==CurrentThread::tid();}
    void assertInLoopThread()
    {
      if(!isInLoopThread())
      {
        abortNotInLoopThread();
      }
    }
  private:
    typedef void(*Functor)();
    typedef vector<Channel*> ChannelList;
    typedef vector<Functor> FunctorList;

    void abortNotInLoopThread();
    void doPendingFunctors();

    void handRead();

    pid_t threadId_;
    bool looping_;
    bool quit_;
    TimeStamp pollReturnTime_;
    bool callingPendingFunctors_;

    scoped_ptr<Poller> poller_;
    scoped_ptr<TimeQueue> timeQueue_;

    struct eventfd wakeupFd_;
    //Channel wakeupChannel_;
    scoped_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    Mutex mutex_;
    FunctorList pendingFunctors_;

};
    
}
#endif