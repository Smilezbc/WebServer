#ifndef WEBSERVER_POLLER_H
#define WEBSERVER_POLLER_H

#include "logging/Logger.h"

#include<map>
#include<boost/noncopyable.hpp>
#include<vector>

namespace webServer
{
  
class Channel;
class EventLoop;

class Poller : boost::noncopyable
{
  public:
    typedef std::vector<struct pollfd> PollfdList;
    typedef std::map<int,Channel*> ChannelMap;
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    ~Poller();

    Timestamp poll(int timeout,ChannelList* channelList);
    void findActiveChannels(int numEvents,ChannelList* activeChannels);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
  private:
    EventLoop* loop_;
    PollfdList pollfds_;
    ChannelMap channels_;
};
  
}
#endif