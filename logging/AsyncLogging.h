#ifndef WEBSERVER_ASYNCLOGGING_H
#define WEBSERVER_ASYNCLOGGING_H

#include "LogStream.h"
#include "../thread/Mutex.h"
#include "../thread/Condition.h"
#include "../thread/Thread.h"
#include "../CountDownLatch.h"

#include<vector>
#include<string>
#include<memory>

namespace webServer
{

class AsyncLogging
{
public:
    AsyncLogging(const std::string& baseName,int rollSize,int flushInterval);
    ~AsyncLogging();

    void start();
    void stop();
    void append(const char* logline,int len);

private:
    typedef webServer::detail::FixedBuffer<webServer::detail::kLargeBufferSize> Buffer;
    typedef std::unique_ptr<Buffer> BufferPtr;
    typedef std::vector<BufferVectorPtr> BufferVector;

    void threadFunc();
    
    bool running_;
    CountDownLatch countDownLatch_;
    const std::string baseName_;
    int rollSize_;
    int flushInterval_;

    MutexLock mutex_;
    Condition cond_;
    BufferVector buffers_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;

    Thread thread_;
};


}
#endif