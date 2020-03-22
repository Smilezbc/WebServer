#ifndef WEBSERVER_COUNTDOWNLATCH_H
#define WEBSERVER_COUNTDOWNLATCH_H

#include "thread/Mutex.h"
#include "thread/Condition.h"

namespace webServer
{

class CountDownLatch
{
public:
    explicit CountDownLatch(int count)
      :mutex(),
      cond_(mutex_),
      count_(count)
    {
    }
    ~CountDownLatch();
    void wait()
    {
        MutexLockGuard lock(mutex_);
        while(count_>0)
        {
            cond_.wait();
        }
    }
    void countDown()
    {
        MutexLockGuard lock(mutex_);
        --count_;
        if(count_==0)
        {
            cond_.notifyAll();
        }
    }
    int getCount()
    {
        MutexLockGuard lock(mutex_);
        return count_;
    }

private:
    MutexLock mutex_;
    Condition cond_;
    int count_;
};


}
#endif