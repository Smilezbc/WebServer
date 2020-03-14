#include "Acceptor.h"

#include "logging/Logging.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <boost/bind.hpp>

using namespace webServer;


Acceptor::Acceptor(EventLoop* loop,InetAddress inetAddress)
  :loop_(loop),
  acceptSocket_(sockets::CreateNonBlockingOrDie()),
  acceptChannel_(loop,acceptSocket_.fd()),
  listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddr(inetAddress);
    acceptChannel_.setReadCallback(boost::bind(Acceptor::handleRead,this));
}
Acceptor::~Acceptor()
{
}
void Acceptor::listen()
{
    loop_->assertInLoopThread();
    listenning_=true;
    acceptSocket_.listen();
    acceptChannel_.enableRead();
}
void Acceptor::handleRead()
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);
    int connfd=acceptSocket_.accept(&peerAddr);
    if(connfd>=0)
    {
        if(newConnectionCallback_)
        {
            newConnectionCallback_(connfd,peerAddr);
        }
        else
        {
            sockets::close(connfd);
        }
        
    }
}

