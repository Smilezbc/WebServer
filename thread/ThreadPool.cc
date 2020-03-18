#include "ThreadPool.h"
#include "Exception.h"

#include <boost/bind.hpp>
#include <assert.h>
#include <stdio.h>

using namespace webServer;

ThreadPool::ThreadPool(std::string name)
  :mutex_(),
  cond_(mutex_),
  name_(name),
  running_(false)
{
}
ThreadPool::~ThreadPool()
{
    if(running_)
    {
        stop();
    }
}
void ThreadPool::start(int numThread)
{
    assert(threads.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for(int i=0;i<numThread;++i)
    {
        char id[32];
        snprintf(id,sizeof id,"%d",i);
        threads_.push_back(new Thread(boost::bind(ThreadPool::runInthread,this),name_+id));
        threads[i].start();
    }
}

void ThreadPool::runInThread()
{
    try
    {
        while(running_)
        {
            Task task(take());
            if(task)
            {
                task();
            }
        }
    }
    catch (const Exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        abort();
    }
}

void ThreadPool::stop()
{
    running_=false;
    cond_.notifyAll();
    for_each(threads.begin(),threads.end(),boost::bind(Thread::join,_1));
}

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock;
    while(tasks.empty() && running_)
    {
        cond_.wait();
    }
    Task task;
    if(!task_.empty())
    {
        task=tasks_.front();
        tasks_.pop();
    }
    return task;
}
void ThreadPool::run(Task& task)
{
    if(threads.emtpy())
    {
        task();
    }
    else
    {
        MutexLockGuard lock(mutex_);
        tasks.push(task);
        cond_.notify();
    }
}
