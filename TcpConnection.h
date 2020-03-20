#ifndef WEBSERVER_TCPCONNECTION_H
#define WEBSERVER_TCPCONNECTION_H

#include "Buffer.h"
#include "Callbacks.h"
#include "InetAddress.h"

#include <boost/any.hpp>
#include <memory>
#include<string>

namespace webServer
{

class Socket;
class Channel;

class TcpConnection
{
public:
    enum StateE
    {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected
    };
    TcpConnection(EventLoop* loop,const std::string& name,int sockfd,InetAddress& localAddress,InetAddress& peerAddress);
    ~TcpConnection();

    EventLoop* getLoop(){ return loop_;}
    const std::string& name(){ return name_;}
    const StateE& state(){ return state_;}
    const InetAddress& localAddr() { return localAddress_;}
    const InetAddress& peerAddr() { return peerAddress_; }
    bool connected() { return state_ == kConnected; }
    

    void setConnectionCallback(const ConnectionCallback& cb){ connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback&cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {writeCompleteCallback_ = cb; }
    void setCloseCallback(const CloseCallback&cb) { closeCallback_ = cb; }

    void send(const std::string& message); 
    void shutdown();
    void setTcpNoDelay(bool on);
    void connectEstablished();
    void connectDestroyed();
    const boost::any& getContext() const{ return context_; };
    boost::any* getMutableContext(){ return &context_; };
    void setContext(const boost::any& context) { context_=context;};
private:

    void handleRead(Timestamp recieveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void setState(const StateE& state) { state_=state; }
    void sendInLoop(const std::string& message);
    void shutdownInLoop();

    EventLoop* loop_;
    const std::string name_;
    StateE state_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
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
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
                                       
}
#endif