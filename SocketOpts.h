#ifndef WEBSERVER_SOCKETOPTS_H
#define WEBSERVER_SOCKETOPTS_H

#include "logging/Logger.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  // snprintf
#include <strings.h>  // bzero
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <endian.h>

namespace webServer
{

namespace sockets
{

typedef sockaddr SA;
const SA* sockaddr_cast(const struct sockaddr_in* addr)
{
  return static_cast<const SA*>(static_cast<const void*>(addr));
}
SA* sockaddr_cast(struct sockaddr_in* addr)
{
  return static_cast<SA*>(static_cast<void*>(addr));
}
int CreateNonBlockingOrDie()
{
    int sockfd=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd<0)
    {
        LOG_SYSFATAL<<"failed in sockets::CreateNonBlockingOrDie()";
    }
    setNonBlockAndCloseOnExec(sockfd);
    return sockfd;
}
void setNonBlockAndCloseOnExec(int sockfd)
{
    //set nonblock
    int flag=::fcntl(sockfd,F_GETFL,0);
    flag|=O_NONBLOCK;
    ::fcntl(sockfd,F_SETFL,flag);

    //set close_on_exec
    flag=::fcntl(sockfd,F_GETFD,0);
    flag|=FD_CLOEXEC;
    ::fcntl(sockfd,F_SETFD,flag);
}

int accept(int sockfd,struct sockaddr_in* addr)
{
    socklen_t socklen=sizeof *addr;
    int connfd=::accept(sockfd,sockaddr_cast(addr),&socklen);

    if (connfd < 0)
    {
        int savedErrno = errno;
        LOG_SYSERR << "Socket::accept";
        switch (savedErrno)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO: // ???
        case EPERM:
        case EMFILE: // per-process lmit of open file desctiptor ???
            // expected errors
            errno = savedErrno;
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        case ENFILE:
        case ENOBUFS:
        case ENOMEM:
        case ENOTSOCK:
        case EOPNOTSUPP:
            // unexpected errors
            LOG_FATAL << "unexpected error of ::accept " << savedErrno;
            break;
        default:
            LOG_FATAL << "unknown error of ::accept " << savedErrno;
            break;
        }
    }
  return connfd;
}
void sockets::close(int sockfd)
{
  if (::close(sockfd) < 0)
  {
    LOG_SYSERR << "sockets::close";
  }
}
void bindOrDie(int sockfd,const struct sockaddr_in& addr)
{
    
    int res=::bind(sockfd,sockaddr_cast(&addr),sizeof addr);
    if(res<0)
    {
        LOG_SYSFATAL <<"sockets::bind()";
    }
}

void shutdownWrite(int sockfd)
{
    int res=::shutdown(sockfd,SHUT_WR);
    if(res<0)
    {
        LOG_SYSERR << "sockets::shutdownWrite";
    }
}
void listenOrDie(int sockefd)
{
    int res=::listen(sockefd,SOMAXCONN);
    if(res<0)
    {
        LOG_SYSFATAL<<"sockets::listenOrDie";
    }
}
struct sockaddr_in getLocalAddr(int sockfd)
{
    struct sockaddr_in addr;
    socklen_t addrLen=sizeof addr;
    bzero(&addr,addrLen);//
    if(::getsockname(sockfd,sockaddr_cast(&addr),&addrLen)<0)
    {
        LOG_SYSERR<< "sockets::getLocalAddr";
    }
    return addr;
}

}
}
#endif