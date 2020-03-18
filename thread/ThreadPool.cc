#include "ThreadPool.h"
#include "Exception.h"

#include <assert.h>
#include <stdio.h>
#include<functional>

using namespace webServer;

ThreadPool::ThreadPool(const std::string& name)
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
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThread);
    for(int i=0;i<numThread;++i)
    {
        char id[32];
        snprintf(id,sizeof id,"%d",i);
        threads_.push_back(new Thread(std::bind(ThreadPool::runInThread,this),name_+id));
        threads_[i].start();
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
    std::for_each(threads_.begin(),threads_.end(),std::bind(Thread::join,1));
}

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(mutex_);
    while(tasks_.empty() && running_)
    {
        cond_.wait();
    }
    Task task;
    if(!tasks_.empty())
    {
        task=tasks_.front();
        tasks_.pop();
    }
    return task;
}
void ThreadPool::run(const Task& task)
{
    if(threads_.empty())
    {
        task();
    }
    else
    {
        MutexLockGuard lock(mutex_);
        tasks_.push(task);
        cond_.notify();
    }
}
