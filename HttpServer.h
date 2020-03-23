#ifndef WEBSERVER_HTTPSERVER_H
#define WEBSERVER_HTTPSERVER_H

#include "TcpServer.h"
#include "TcpConnection.h"
#include "Timestamp.h"
#include "Buffer.h"
#include "Atomic.h"

#include <memory>

namespace webServer
{

class EventLoop;
class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    HttpServer(EventLoop* loop,
               InetAddress inetAddr，
               int maxConnections,
               int idleSeconds);
    ~HttpServer();
    void start();
    void setNumThread(int numThread);
private:
    typedef std::weak_ptr<webServer::TcpConnection> WeakTcpConnectionPtr;
    struct Entry
    {
        Entry(const WeakTcpConnectionPtr& weakConn)
          :weakConn_(weakConn)
        {
        }
        ~Entry()
        {
            webServer::TcpConnectionPtr conn=weakConn_.lock();
            if(conn)
            {
                conn->shutdown();
            }
        }
        WeakTcpConnectionPtr weakConn_;
    };
    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakConnectionList;

    struct SavedData
    {
        SavedData(HttpContext* httpContext,
                  const WeakEntryPtr& weakConn)
          :httpContext_(httpContext),
           weakConn_(weakConn)
        {
        }
        HttpContext* httpContext_;
        WeakEntryPtr weakConn_;
    }

    void onConnection(TcpConnectionPtr&);
    void onMessage(TcpConnectionPtr&,Buffer*,Timestamp);
    void onRequest(TcpConnectionPtr& conn,const HttpRequest& request);
    void onHttpRequest(const HttpRequest&,HttpResponse*);
    void onTimer();

    TcpServer server_;
    AtomicInt32 numConnected_;
    const int kMaxConnections_;
    WeakConnectionList connectionBuckets_;
};

}

#endif