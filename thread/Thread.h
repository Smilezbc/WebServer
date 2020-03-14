#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H
#include "Atomic.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <pthread.h>
#include<string>

namespace webServer
{

namespace CurrentThread
{
    //tid_t tid();
    pid_t tid();
    const char* name();
    bool isMainThread();
}

class Thread
{
public:
    typedef void (*ThreadFunc)();

    Thread(ThreadFunc func,const std::string& name = std::string());
    ~Thread();

    void start();
    void join();

    bool started(){return start_;}
    pid_t tid(){return *pTid_;}
    const std::string& name() const{return name_;}
    static int numThread() {return numThread_;}
private:
    bool start_;
    bool join_;
    pthread_t pthreadId_;//pthread_create()返回的线程id的类型
    shared_ptr<pid_t> pTid_;
    ThreadFunc func_;
    std::string name_;

    static AtomicInt32 numThread_;
};

}

#endif