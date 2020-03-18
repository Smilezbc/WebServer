#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace webServer;

HttpServer::HttpServer(EventLoop* loop,InetAddress inetAddr)
  :server_(loop,inetAddr)
{
    server_.setConnectionCallback(std::bind(HttpServer::onConnection,this));
    server_.setMessageCallback(std::bind(HttpServer::onMessage,this));
}

HttpServer::~HttpServer()
{

}

void HttpServer::setNumThread(int numThread)
{
    server_.setNumThread(numThread);
}

void HttpServer::start()
{
    server_.start();
}

void HttpServer::onConnection(TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn.setContext(new HttpContext());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,Buffer* buffer,TimeStamp receiveTime)
{
    HttpContext* httpContext=boost::any_cast<HttpContext>(conn->getMutableContext());
    if(!httpContext->parseRequest(buffer,receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    if(httpContext->gotAll())
    {
        onRequest(conn,httpContext->request());
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn,HttpRequest& request)
{
    const string& connection=request.getHead("Connection");
    bool close = (connection == "Close" || (request.getVersion() == HttpRequest::kHttp10 && connection!="Keep-Alive"))
    HttpResponse response(close);
    onHttpRequest(request,&response);
    Buffer buffer;
    response.appendToBuffer(&buffer);
    conn->send(&buffer);
    if(response.closeConnection())
    {
        conn->shutdown();
    }
}
void HttpServer::onHttpRequest(const HttpRequest& request,HttpResponse* response)
{
    const string& path=request.path();
    if(path=="/")
    {
        response.setStatusCode(HttpResponse::k200Ok);
        response.setStatusMessage("Ok");
        response.setConnectionClose(true);
        response.setContentType("text/html");
        response.setBody("<html>good morning,my princess!</html>");
    }
    else
    {
        response.setStatusCode(HttpResponse::k404NotFound);
        response.setStatusMessage("Sorry,Not Found");
        response.setConnectionClose(true);
    }
    
}

