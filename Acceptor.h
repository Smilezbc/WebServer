#ifndef WEBSERVER_ACCEPTOR_H
#define WEBSERVER_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "Channel.h"
#include "Socket.h"

namespace webServer
{

class EventLoop;
class InetAddress;
class Socket;
class Channel;
class Acceptor
{
public:
    typedef void (*NewConnectionCallback)(int sockfd,const InetAddress& inetAddress);
    Acceptor(EventLoop* loop,InetAddress inetAddress);

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {newConnectionCallback_=cb;}
    bool listenning();
    void listen();
private:
    void handleRead();
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    bool listenning_;
    NewConnectionCallback newConnectionCallback_;

};

}
#endif