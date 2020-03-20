#include "TcpServer.h"

#include "logging/Logger.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "SocketOpts.h"

#include <functional>

#include <stdio.h>  

using namespace webServer;

TcpServer::TcpServer(EventLoop* loop,InetAddress& listenAddr)
  :loop_(loop),
  name_(listenAddr.toHostAndPost()),
  acceptor_(new Acceptor(loop,listenAddr)),
  threadPool_(new EventLoopThreadPool(loop)),
  nextConnId_(1),
  started_(false)
{
    acceptor_->setNewConnectionCallback(std::bind(TcpServer::newConnection,this,_1,_2));
}
TcpServer::~TcpServer()
{

}

void TcpServer::setNumThread(int num)
{
    assert(num>=0);
    threadPool_->setNumThreads(num);
}

//thread safe
void TcpServer::start()
{
    if(!started_)
    {
        started_=true;
        threadPool_->start();
    }
    if(!acceptor_->listenning())
    {
        //acceptor_.listen();
        loop_->runInLoop(std::bind(&Acceptor::listen,get_pointer(acceptor_)));
    }
}

void TcpServer::newConnection(int connfd,InetAddress& peerAddr)
{
    EventLoop* loop=threadPool_->getNextLoop();

    char id[32];
    snprintf(id,sizeof id,"#%d",nextConnId_);
    ++nextConnId_;
    std::string connName=name_+id;
    
    LOG_INFO << "TcpServer::newConnection [" << name_
           << "] - new connection [" << connName
           << "] from " << peerAddr.toHostAndPost();

    InetAddress localAddr(sockets::getLocalAddr(connfd));
    TcpConnectionPtr conn(new TcpConnection(loop,connName,connfd,localAddr,peerAddr));
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,_1));
    connections_[connName]=conn;
    loop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
           << "] - connection " << conn->name();
    connections_.erase(conn->name());
    EventLoop* connLoop=conn->getLoop();
    connLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
}