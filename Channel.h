#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H

#include "Callbacks.h"

namespace webServer
{

class Channel
{
  public:
    Channel(EventLoop* loop,int fd);
    ~Channel();

    void handleEvent(Timestamp time_);

    void setReadCallback(const ReadEventCallback& cb){readCallback_=cb;}
    void setWriteCallback(const EventCallback& cb){writeCallback_=cb;}
    void setErrorCallback(const EventCallback& cb){errorCallback_=cb;}
    void setCloseCallback(const EventCallback& cb){closeCallback_=cb;}

    void enableRead(){events_ |= kReadEvent;update();}
    void enableWrite(){events_ |= kWriteEvent;update();}
    void disableWrite(){events_ &= ~kWriteEvent;update();}
    void disableAll(){events_=kNoneEvent;update();}
    bool isWriting() const { return events_ & kWriteEvent;}
    EventLoop* loop(){return loop_;}

    int fd(){return fd_;}
    int events(){return events_;}

    int index(){return index_;}
    void setIndex(int idx){index_=idx;}

    void setRevents(int revents){revents_=revents;}
    bool isNoneEvent(){return events_==kNoneEvent;}
    EventLoop* loop(){return loop_;}
  private:
    void update();
    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;

    int index_;
    bool eventHandling_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

};
  
}

#endif
