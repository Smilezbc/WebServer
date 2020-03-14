#ifndef WEBSERVER_CONDITION_H
#define WEBSERVER_CONDITION_H
#include "Mutex.h"

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
        pthread_cond_destory(&cond_);
    }
    void wait()
    {
        pthread_cond_wait(&cond_,mutex_.getPthreadMutex());
    }
    bool waitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv.sec+=seconds;
        return ETIMEOUT==pthread_cond_timedwait(&cond_,mutex_.getPthreadMutex(),&abstime);
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
    pthreda_cond_t cond_;
    MutexLock& mutex_;
};

#endif