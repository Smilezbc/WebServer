#ifndef WEBSERVER_SOCKETOPTS_H
#define WEBSERVER_SOCKETOPTS_H
#include "logging/Logger.h"
namespace webServer
{
namespace sockets
{

typedef sockaddr SA;
int CreateNonBlockingOrDie()
{
    int sockfd=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd<0)
    {
        LOG_SYSFATAL<<"failed in sockets::CreateNonBlockingOrDie()"
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

void setReuseAddr(int sockfd)
{
    ::setSockOpts()
}
int accept(int sockfd,struct sockaddr_in* addr_)
{
    socklen_t socklen=sizeof *addr;
    int connfd=::accept(sockfd,static_cast<SA*>(addr),socklen);

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
    int res=::bind(sockfd,static_cast<const SA*>(&addr),sizeof addr);
    if(res<0)
    {
        LOG_SYSFATAL <<"sockets::bind()";
    }
}

}
}
#endif