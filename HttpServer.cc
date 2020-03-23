#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpContext.h"
#include "logging/Logger.h"
#include "EventLoop.h"

#include<functional>

using namespace webServer;

HttpServer::HttpServer(EventLoop* loop,
                       InetAddress inetAddr,
                       int maxConnection,
                       int idleSeconds)
  :server_(loop,inetAddr),
  numConnected_(0),
  kMaxConnections_(maxConnection),
  connectionBuckets_(idleSeconds)
{
    server_.setConnectionCallback(std::bind(HttpServer::onConnection,this,_1));
    server_.setMessageCallback(std::bind(HttpServer::onMessage,this,_1,_2,_3));
    connectionBuckets_.resize(idleSeconds);
    loop->runEvery(1.0,std::bind(HttpServer::onTimer,this));
}

HttpServer::~HttpServer()
{

}
void HttpServer::onTimer()
{
  connectionBuckets_.push_back(Bucket());
}

void HttpServer::setNumThread(int numThread)
{
    server_.setNumThread(numThread);
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer starts listenning on " << server_.name();
    server_.start();
}

void HttpServer::onConnection(TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        if(numConnected_.incrementAndGet()>kMaxConnections_)
        {
            conn->shutdown();
        }
        else
        {
            EntryPtr entry(new Entry(conn));
            connectionBuckets_.back().insert(entry);
            WeakEntryPtr weakEntry(entry);
            conn->setContext(new SavedData(new HttpContext(),weakEntry));
        }
    }
    else
    {
        numConnected_.decrement();
    }
}

void HttpServer::onMessage(TcpConnectionPtr& conn,Buffer* buffer,Timestamp receiveTime)
{
    SavedData* savedData=boost::any_cast<SavedData>(conn->getMutableContext());
    HttpContext* httpContext=savedData->httpContext_;
    WeakEntryPtr weakEntry=savedData->weakConn_;
    EntryPtr entry(weakEntry.lock());
    if(entry)
    {
        connectionBuckets_.back().insert(entry);
    }
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

void HttpServer::onRequest(TcpConnectionPtr& conn,const HttpRequest& request)
{
    const std::string& connection=request.getHead("Connection");
    bool close = (connection == "Close" || (request.version() == HttpRequest::kHttp10 && connection!="Keep-Alive"));
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
    const std::string& path=request.path();
    if(path=="/")
    {
        response->setStatusCode(HttpResponse::k200Ok);
        response->setStatusMessage("Ok");
        response->setCloseConnection(true);
        response->setContentType("text/html");
        response->setBody("<html>good morning,my princess!</html>");
    }
    else
    {
        response->setStatusCode(HttpResponse::k404NotFound);
        response->setStatusMessage("Sorry,Not Found");
        response->setCloseConnection(true);
    }
    
}

