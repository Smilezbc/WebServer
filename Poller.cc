#include "Poller.h"
#include "TimeStamp.h"
#include "Channel.h"
using namespace webServer;

Poller::Poller(EventLoop* loop)
  :loop_(loop)
{
}
Poller::~Poller()
{
}
Poller::poll(int timeoutMs,ChannelList& activeChannels)
{
    int numEvents=::poll(timeoutMs,pollfds_.begin(),pollfds_.size());
    TimeStamp now=TimeStamp::now();
    if(numEvents>0)
    {
        LOG_TRACE << numEvents <<" events happened";
        findActiveChannels(numEvents,activeChannels);
    }
    else if(numEvents==0)
    {
        LOG_TRACE << "nothing happened";
    }
    else
    {
        LOG_SYSERR << "Poller::poll()";
    }
    return now;
}
void Poller::findActiveChannels(int numEvents,ChannelList& activeChannels)
{
    for(PollfdList::const_iterator it=pollfds_.begin();numEvents>0 && it!=pollfds_.end();++it)
    {
        if(it->revents)
        {
            --numEvents;
            ChannelMap::const_iterator ch=channels_.find(it->fd);
            assert(ch!=channels_.end());
            Channel channel=ch->second;
            assert(channel.fd() == it->fd);
            channel.setRevents(it->revents);
            activeChannels.push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel* channel)
{
    assertInLoopThread();//
    LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
    if(channel->index() < 0 )
    {
        assert(channels_.find(channel.fd())==channels_.end());
        struct pollfd pfd;
        pfd.fd=channel.fd();
        pfd.event=channel.event();
        pfd.revent=0;
        pollfds_.push_back(pfd);
        channels_[channel->fd()]=channel;
    }
    else
    {
      assert(channels_.find(channel.fd())!=channels_.end());
      assert(channels_[channel->fd()]==channel);
      int index=channel->index()
      assert(index>=0 && index<pollfds_.size());
      struct pollfd& pfd=pollfds_[index];
      assert(pfd.fd==channel->fd() || pfd.fd == -channel->fd()-1);
      pfd.event=channel->event();
      if(channel->isNoneEvent())
      {
          pfd.fd=-channel->fd()-1;
      }
    }
    
}
void Poller::removeChannel(Channel* channel)
{
    assertInLoopThread();//
    LOG_TRACE << "fd = " << channel->fd();
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());
    int idx=channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    struct pollfd& pfd=pollfds_[channel->index()];
    assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
    size_t n=channels_.erase(channel->fd());
    assert(n==1)
    if(idx==pollfds_.size()-1){
        pollfds_.pop_back();
    }
    else
    {
        int lastfd=pollfds_.back().fd;
        iter_swap(pollfds_.begin()+idx,pollfds_.end()-1);
        if(lastfd<0) astfd=-lastfd-1;
        channels_[lastfd]->setIndex(idx);
        pollfds_.pop_back();
    }
    
}
