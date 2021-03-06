#include "TcpConnection.h"

#include "logging/Logger.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketOpts.h"

#include<memory>
#include <errno.h>

using namespace webServer;
using namespace std;

TcpConnection::TcpConnection(EventLoop* loop,const string& name,int sockfd,InetAddress& localAddress,InetAddress& peerAddress)
  :loop_(loop),
  name_(name),
  state_(kConnecting),
  socket_(new Socket(sockfd)),
  channel_(new Channel(loop,sockfd)),
  localAddress_(localAddress),
  peerAddress_(peerAddress)
{
    LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this<< " fd=" << sockfd;
    channel_->setReadCallback( bind(&TcpConnection::handleRead,this,_1) );
    channel_->setWriteCallback( bind(&TcpConnection::handleWrite,this) );
    channel_->setCloseCallback( bind(&TcpConnection::handleClose,this) );
    channel_->setErrorCallback( bind(&TcpConnection::handleError,this) );
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd();
}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_==kConnecting);
    state_=kConnected;
    channel_->enableRead();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    loop_->assertInLoopThread();
    assert(state_==kConnected || state_==kDisconnecting);
    state_=kDisconnected;
    channel_->disableAll();
    connectionCallback_(shared_from_this());

    loop_->removeChannel(get_pointer(channel_));
}


void TcpConnection::setTcpNoDelay(bool on)
{
    socket_->setNoDelay(on);
}

void TcpConnection::shutdown()
{
    if(state_==kConnected)
    {
        setState(kDisconnecting);
        if(loop_->isInLoopThread())
        {
            shutdownInLoop();
        }
        else
        {
            loop_->queueInLoop(bind(TcpConnection::shutdownInLoop,this));
        }
    }
}

void TcpConnection::shutdownInLoop()
{
    loop_->assertInLoopThread();
    if(!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
}

void TcpConnection::send(const std::string& message)
{
    if(state_==kConnected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            loop_->queueInLoop(bind(TcpConnection::sendInLoop,this,message));
        }
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    loop_->assertInLoopThread();
    ssize_t nwrote=0;
    if(!channel_->isWriting() && outputBuffer_.readableBytes()==0)
    {
        nwrote=::write(channel_->fd(),message.data(),message.size());
        if(nwrote>=0)
        {
            if(nwrote < static_cast<ssize_t>(message.size()))
            {
                LOG_TRACE << "I am going to write more data";
            }
            else if(writeCompleteCallback_)
            {
                loop_->queueInLoop(bind(writeCompleteCallback_,shared_from_this()));
            }
        }
        else
        {
            nwrote=0;
            if(errno!=EWOULDBLOCK)//EWOULDBLOCK的原因：socket的发送缓冲区的低水位线没有达到
            {
                LOG_SYSERR << "TcpConnection::sendInLoop";
            }
        }
    }
    assert(nwrote>=0);
    if(nwrote < static_cast<ssize_t>(message.size()))
    {
        outputBuffer_.append(message.data()+nwrote, message.size()-nwrote);
        if (!channel_->isWriting()) 
        {
            channel_->enableWrite();
        }
    }
}

void TcpConnection::handleRead(Timestamp recieveTime)
{
    loop_->assertInLoopThread();
    int savedErrno;
    size_t n=inputBuffer_.readfd(socket_->fd(),&savedErrno);
    if(n>0)
    {
        messageCallback_(shared_from_this(),&inputBuffer_,recieveTime);
    }
    else if(n==0)
    {
        handleClose();
    }
    else
    {
        errno=savedErrno;
        LOG_SYSERR << "TcpConnection::handleRead";//
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    loop_->assertInLoopThread();
    if(channel_->isWriting())
    {
        ssize_t n=write(socket_->fd(),outputBuffer_.peek(),outputBuffer_.readableBytes());
        if(n>0)
        {
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes()==0)
            {
                channel_->disableWrite();
                if(writeCompleteCallback_)
                {
                    //writeCompeleCallback();
                    loop_->queueInLoop(bind(writeCompleteCallback_, shared_from_this()));
                }
                if(state_==kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
            else
            {
                LOG_TRACE << "I am going to write more data";
            }
        }
        else
        {
            LOG_SYSERR << "TcpConnection::handleWrite";
        }
    }
    else 
    {
        LOG_TRACE << "Connection is down, no more writing";
    }
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    assert(state_==kConnected || state_==kDisconnecting);
    channel_->disableAll();
    closeCallback(shared_from_this());
}

void TcpConnection::handleError()
{
    loop_->assertInLoopThread();
    int err=sockets::getSocketError(socket_->fd());
    LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}