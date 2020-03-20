#ifndef WEBSERVER_CALLBACKS_H
#define WEBSERVER_CALLBACKS_H

#include "Timestamp.h"

#include<memory>
#include<functional>

namespace webServer
{
    
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

template<typename T>
inline T* get_pointer(const std::shared_ptr<T>& ptr)
{
  return ptr.get();
}

template<typename T>
inline T* get_pointer(const std::unique_ptr<T>& ptr)
{
  return ptr.get();
}

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void()> EventCallback;
typedef std::function<void(Timestamp)>ReadEventCallback;

typedef std::function<void (TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (TcpConnectionPtr&,Buffer*,Timestamp)> MessageCallback;
typedef std::function<void (TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (TcpConnectionPtr&)> CloseCallback;

}
#endif