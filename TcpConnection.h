#ifndef WEBSERVER_TCPCONNECTION_H
#define WEBSERVER_TCPCONNECTION_H
#include "Buffer.h"
#include "Callbacks.h"
#include "InetAddress.h"

#include <boost/any.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace webServer
{

class TcpConnection
{
public:
    enum StateE
    {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected
    }
    TcpConnection(EventLoop* loop,const string& name,int sockfd,InetAddress& localAddress,InetAddress& peerAddress);
    ~TcpConnection();

    EventLoop* getLoop(){ return loop_;}
    const std::string& name(){ return name_;}
    const StateE& state(){ return state_;}
    const InetAddress& localAddr() { return localAddress_;}
    const InetAddress& peerAddr() { return peerAddress_; }
    bool connected() { return state_ == kConnected; }
    

    void setConnectionCallback(ConnectionCallback& cb){ connectionCallback_ = cb; }
    void setMessageCallback(MessageCallback&cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(WriteCompleteCallback& cb) {writeCompleteCallback_ = cb; }
    void setCloseCallback(CloseCallback&cb) { closeCallback_ = cb; }

    void send(const string& message); 
    void shutdown();
    void setTcpNoDelay(bool on);
    void connectEstablished();
    void connectDestroyed();
    const boost::any& getContext() const{ return context_; };
    boost::any* getMutableContext(){ return &context_; };
    void setContext(boost::any& context) { context_=context;};
private:

    void handleRead(TimeStamp recieveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void setState(StateE& state) { return state_=state; }
    void sendInLoop(const string& message);
    void shutdownInLoop();

    EventLoop* loop_;
    const std::string name_;
    StateE state_;

    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    InetAddress localAddress_;
    InetAddress peerAddress_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback  writeCompleteCallback_;
    CloseCallback closeCallback_;
    boost::any context_;

};

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
}
#endif