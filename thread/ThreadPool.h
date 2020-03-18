#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <queue>
namespace webServer
{

class ThreadPool
{
public:

    typedef void (*Task)();
    ThreadPool(const std::string& name);
    ~ThreadPool();

    void start(int numThread);
    void stop();
    void run(const Task& task);
private:

    void runInThread();
    Task take();

    MutexLock mutex_;
    Condition cond_;
    std::string name_;
    boost::ptr_vector<Thread> threads_;
    std::queue<Task> tasks_;
    bool running_;
};
    
}
#endif