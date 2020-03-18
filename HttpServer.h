#ifndef WEBSERVER_HTTPSERVER_H
#define WEBSERVER_HTTPSERVER_H

#include "TcpServer.h"
#include "TcpConnection.h"
#include "TimeStamp.h"
#include "Buffer.h"

class HttpServer
{
public:
    HttpServer(EventLoop* loop,InetAddress inetAddr);
    ~HttpServer();
    void start();
    void setNumThread(int numThread);
private:

    void onConnection(TcpConnetionPtr&);
    void onMessage(TcpConnetionPtr&,Buffer*,TimeStamp);
    void onRequest();
    void onHttpRequest(const HttpRequest&,HttpResponse*);

    TcpServer server_;
};

#endif