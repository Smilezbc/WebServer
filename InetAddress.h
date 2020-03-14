#ifndef WEBSERVER_INETADDRESS_H
#define WEBSERVER_INETADDRESS_H
class InetAddress
{
public:
    InetAddress(int port);
    InetAddress(string addr,int port);
    InetAddress(const struct sockaddr_in& addr);

    ~InetAddress();
    
    struct sockaddr_in getAddress();
    void setAddress(const struct sockaddr_in&);

    

private:
    struct sockaddr_in addr_;
};


#endif