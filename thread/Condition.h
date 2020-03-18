#ifndef WEBSERVER_CONDITION_H
#define WEBSERVER_CONDITION_H

#include "Mutex.h"

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <errno.h>

namespace webServer
{

class Condition
{

public:
    Condition(MutexLock& mutex)
      :mutex_(mutex)
    {
        //pthread_cond_init(&mutex_.mutex(),&cond_);
        pthread_cond_init(&cond_,nullptr);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond_);
    }
    void wait()
    {
        pthread_cond_wait(&cond_,mutex_.getPthreadMutex());
    }
    bool waitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += seconds;
        return ETIMEDOUT==pthread_cond_timedwait(&cond_,mutex_.getPthreadMutex(),&abstime);
    }
    void notify()
    {
        pthread_cond_signal(&cond_);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&cond_);
    }
    
private:
    pthread_cond_t cond_;
    MutexLock& mutex_;
};
    
}
#endif