#ifndef WEBSERVER_CALLBACKS_H
#define WEBSERVER_CALLBACKS_H

#include "Timestamp.h"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace webServer
{
    class Buffer;
    class TcpConnection;
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef boost::function<void()> TimerCallback;
    typedef boost::function<void()> EventCallback;
    typedef boost::function<void(Timestamp)>ReadEventCallback;

    typedef boost::function<void (TcpConnectionPtr&)> ConnectionCallback;
    typedef boost::function<void (TcpConnectionPtr&,Buffer*,Timestamp)> MessageCallback;
    typedef boost::function<void (TcpConnectionPtr&)> WriteCompleteCallback;
    typedef boost::function<void (TcpConnectionPtr&)> CloseCallback;
}
#endif