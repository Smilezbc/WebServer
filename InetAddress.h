#ifndef WEBSERVER_INETADDRESS_H
#define WEBSERVER_INETADDRESS_H

#include <string>
#include <netinet/in.h>

namespace webServer
{

class InetAddress
{
    
public:
    InetAddress(int port);
    InetAddress(std::string addr,int port);
    InetAddress(const struct sockaddr_in& addr);

    ~InetAddress();
    
    struct sockaddr_in getAddress();
    void setAddress(const struct sockaddr_in&);
    std::string toHostAndPost()const;
    
private:
    struct sockaddr_in addr_;
};
    
}

#endif