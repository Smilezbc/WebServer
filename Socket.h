#ifndef WEBSERVER_SOCKET_H
#define WEBSERVER_SOCKET_H
namespace webServer
{

class InetAddress;
class Socket
{
public:
    explicit Socket(int fd)
      :fd_(fd)
    {
    }
    ~Socket();
    int fd(){return fd_;}
    void setReuseAddr(bool on);
    void bindAddr(InetAddress inetAddr);
    int accept(InetAddress* inetAddr);
private:
    int fd_;
};


}
#endif