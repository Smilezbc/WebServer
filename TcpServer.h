#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H

#include "Callbacks.h"
#include "TcpConnection.h"

#include <map>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace webServer
{

class TcpServer
{
public:

    TcpServer(EventLoop* loop,InetAddress& listenAddr);
    ~TcpServer();

    void setNumThread(int num);
    void start();
    void setConnectionCallback(ConnectionCallback& cb){connectionCallback_=cb;}
    void setMessageCallback(MessageCallback& cb){ messageCallback_=cb;}
    void setWriteCompleteCallback(WriteCompleteCallback& cb){ writeCompleteCallback_=cb;}

private:

    void newConnection();
    void removeConnection(const TcpConnectionPtr&);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    typedef map<std::string,TcpConnectionPtr> ConnectionMap;
    EventLoop* loop_;
    const string name_;

    boost::scoped_ptr<Acceptor> acceptor_;
    boost::scoped_ptr<EventLoopThreadPool> threadPool_;

    bool started_;
    int nextConnId;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    ConnectionMap connections_;
};

    
}
#endif