#include "Thread.h"
namespace
{
__thread pid_t t_cachedTid = 0;
}

pid_t CurrentThread::tid()
{
    if(t_cachedTid==0)
    {
        t_cachedTid=::gettid();
    }
    return t_cachedTid;
}
