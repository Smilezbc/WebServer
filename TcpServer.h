#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H

#include "Callbacks.h"
#include "TcpConnection.h"

#include <map>
#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

namespace webServer
{

class Acceptor;
class EventLoopThreadPool;

class TcpServer
{
public:

    TcpServer(EventLoop* loop,InetAddress& listenAddr);
    ~TcpServer();

    void setNumThread(int num);
    const std::string& name() const {return name_;}
    void start();
    void setConnectionCallback(const ConnectionCallback& cb){connectionCallback_=cb;}
    void setMessageCallback(const MessageCallback& cb){ messageCallback_=cb;}
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompleteCallback_=cb;}

private:

    void newConnection(int connfd,InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr&);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<EventLoopThreadPool> threadPool_;

    bool started_;
    int nextConnId_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    ConnectionMap connections_;
};

    
}
#endif