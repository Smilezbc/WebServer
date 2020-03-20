#include "InetAddress.h"

#include "SocketOpts.h"

#include <strings.h>  // bzero
#include <netinet/in.h>


using namespace webServer;

std::string InetAddress::toHostAndPost() const
{
    char buf[32];
  sockets::toHostPort(buf, sizeof buf, addr_);
  return buf;  
}