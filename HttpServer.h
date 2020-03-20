#ifndef WEBSERVER_HTTPSERVER_H
#define WEBSERVER_HTTPSERVER_H

#include "TcpServer.h"
#include "TcpConnection.h"
#include "Timestamp.h"
#include "Buffer.h"


namespace webServer
{

class EventLoop;
class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    HttpServer(EventLoop* loop,InetAddress inetAddr);
    ~HttpServer();
    void start();
    void setNumThread(int numThread);
private:

    void onConnection(TcpConnectionPtr&);
    void onMessage(TcpConnectionPtr&,Buffer*,Timestamp);
    void onRequest(TcpConnectionPtr& conn,const HttpRequest& request);
    void onHttpRequest(const HttpRequest&,HttpResponse*);

    TcpServer server_;
};

}

#endif