#include "Socket.h"
#include "InetAddress.h"
#include "SocketOpts.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>


using namespace webServer;

void Socket::setReuseAddr(bool on)
{
    int optval=on?1:0;
    ::setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
}
void Socket::bindAddr(InetAddress inetAddr)
{
    sockets::bindOrDie(fd_,inetAddr.getAddress());
}
int Socket::accept(InetAddress* peerAddr)
{
    struct sockaddr_in addr;
    ::bzero(&addr,sizeof addr);

    int connfd=sockets::accept(fd_,&addr);

    if(connfd>=0)
    {
        peerAddr->setAddress(addr);
    }
    return connfd;

}
void Socket::listen()
{
    sockets::listenOrDie(fd_);
}
void Socket::shutdownWrite()
{
    sockets::shutdownWrite(fd_);
}

void Socket::setNoDelay(bool on)
{
    int optval=on?1:0;
    ::setsockopt(fd_,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof optval);
}