#include "Thread.h"

#include <boost/weak_ptr.hpp>

#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
namespace webServer
{
namespace CurrentThread
{
    __thread const char* t_threadName="unkown";
}

}
namespace
{
pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}
struct ThreadData
{
    webServer::Thread::ThreadFunc func_;
    string name_;
    weak_ptr<pid_t> wkTid_;
    ThreadData(webServer::Thread::ThreadFunc func,string& name,shared_ptr<pid_t> pTid)
    :func_(fun),
    name_(name),
    wkTid_(pTid)
    {
    }
    void runInThread()
    {
        pid_t tid=gettid();
        shared_ptr<pid_t> ptid=wkTid.lock();

        if(ptid)//如果创建子线程的父线程还存在的话，就把tid存到父线程中
        {
            *ptid=tid;
            ptid.reset();
        }

        webServer::CurrentThread::t_threadName=name_.empty()?"webServerThread":name_.c_str();//保证了每个线程的t_threadName都被设置了
        ::prctl(PR_SET_NAME, muduo::CurrentThread::t_threadName);
        func_();
        webServer::CurrentThread::t_threadName="finished";
    }
};
void* startThread(void* threadData)//每个thread 的主函数
{
    ThreadData* data=static_cast<ThreadData*>(threadData);
    data->runInThread();
    delete data;//当线程运行结束时，将存储线程信息的data释放掉
    return nullptr;
}
void atfork()
{
    t_cacheTid=gettid();
    webServer::CurrentThread::t_ThreadName="main";
}
class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        webServer::CurrentThread::t_ThreadName="main";
        pthread_atfork(nullptr,nullptr,atfork);
    }
}
ThreadNameInitializer init; //作用：设置主线程的t_ThreadName

}
using namespace webServer;

Thread::Thread(Thread::Func func,const string& name)
  :start_(false),
  join_(false),
  pthreadId_(0),
  pTid_(new pid_t(0)),
  func_(func_),
  name_(name);
{
    numThread_.increment();
}
Thread::~Thread()
{
    if(start_ && !join_)
    {
        pthread_detch(pthreadId_);
    }
}
void Thread::start()
{
    assert(!start_);
    start_=true;
    ThreadData* threadData =  new ThreadData(func_,name_,pTid_);
    if(pthread_create(&pthreadId_,nullptr,startThread,threadData))//传递给pthread_create()的函数必须是void*(*func)(void*)的函数指针
    {
        start_=false;
        delete threadData;
        abort();
    }
}
void Thread::join()
{
    assert(!join_);
    join_=true;
    pthread_join(pthreadId_);
}

pid_t CurrentThread::tid()
{
    if(t_cacheTid==0)
    {
        t_cacheTid=gettid();
    }
    return t_cacheTid;

}
const char* CurrentThread::name()
{
    return t_threadName;
}
bool CurrentThread::isMainThread()
{
    return tid()==::getpid();
}