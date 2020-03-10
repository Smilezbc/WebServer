#ifndef WEBSERVER_POLLER_H
#define WEBSERVER_POLLER_H

#include<boost/noncopyable.hpp>
#include<vector>
#include<map>
#include "logging/Logger.h"
namespace webServer
{
class Channel;
class Poller : boost::noncopyable
{
  public:
    typedef vector<struct pollfd> PollfdList;
    typedef map<int,Channel*> ChannelMap;
    typedef vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    ~Poller();

    void poll(TimeStamp timeout,ChannelList channelList);
    void findActiveChannels(ChannelList activeChannels);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
  private:
    EventLoop* loop_;
    PollfdList pollfds_;
    ChannelMap channels_;
};
  
}
#endif