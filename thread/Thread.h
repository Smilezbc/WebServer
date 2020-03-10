#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H
namespace CurrentThread
{
    //tid_t tid();
    pid_t tid();
    const char* name();
    bool isMainThread();
}
#endif