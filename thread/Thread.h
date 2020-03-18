#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H

#include "Atomic.h"

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include<string>

namespace webServer
{

namespace CurrentThread
{
    pid_t tid();
    const char* name();
    bool isMainThread();
}

class Thread
{
public:
    typedef std::function<void()> ThreadFunc;

    Thread(ThreadFunc func,const std::string& name = std::string());
    ~Thread();

    void start();
    void join();

    bool started(){return start_;}
    pid_t tid(){return *pTid_;}
    const std::string& name() const{return name_;}
    static int numThread() {return numThread_.get();}
private:
    bool start_;
    bool join_;
    pthread_t pthreadId_;//pthread_create()返回的线程id的类型
    std::shared_ptr<pid_t> pTid_;
    ThreadFunc func_;
    std::string name_;

    static AtomicInt32 numThread_;
};

}

#endif
