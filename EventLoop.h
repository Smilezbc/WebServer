
#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include "logging/Logger.h"
#include "thread/Mutex.h"
#include "thread/Thread.h"
#include "Callbacks.h"

#include<vector>
#include<memory>

namespace webServer
{

class Channel;
class Poller;
class TimeQueue;

class EventLoop : boost::noncopyable
{
  public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void updateChannel(Channel*);
    void removeChannel(Channel*);

    void runAt(TimerCallback cb,Timestamp when);
    void runAfter(TimerCallback cb,Timestamp when);
    void runEvery(TimerCallback cb,double interval);
    void cancel(TimerId timerId);//用Timer的序列号来取消某个计时器；用Timer的地址来取消timer是不可靠的

    void wakeup();
    void runInLoop(Functor functor);
    void queueInLoop(Functor functor);

    bool isInLoopThread(){return threadId_==CurrentThread::tid();}
    void assertInLoopThread()
    {
      if(!isInLoopThread())
      {
        abortNotInLoopThread();
      }
    }
  private:
    typedef std::vector<Channel*> ChannelList;
    typedef std::vector<Functor> FunctorList;

    void abortNotInLoopThread();
    void doPendingFunctors();

    void handRead();

    pid_t threadId_;
    bool looping_;
    bool quit_;
    Timestamp pollReturnTime_;
    bool callingPendingFunctors_;

    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimeQueue> timeQueue_;

    int wakeupFd_;
    //Channel wakeupChannel_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    MutexLock mutex_;
    FunctorList pendingFunctors_;

};
    
}
#endif