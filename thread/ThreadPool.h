#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H
#include "Condition.h"
#include "Mutex.h"
#include "Thread.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <deque>
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

    void runInthread();
    Task take();

    MutexLock mutex_;
    Condition cond_;
    string name_;
    boost::ptr_vector<Thread> threads_;
    queue<Task> tasks_;
    bool running_;
}
    
}
#endif