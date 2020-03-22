#include "AsyncLogging.h"
#include "LogFile.h"

#include<functional>

using namespace webServer;
using namespace std;

AsyncLogging::AsyncLogging(const string& baseName,int rollSize,int flushInterval=3)
  :running_(false),
  countDownLatch_(1),
  baseName_(baseName),
  rollSize_(rollSize),
  flushInterval_(flushInterval),
  mutex_(),
  cond_(mutex_),
  buffers_(),
  currentBuffer_(new Buffer),
  nextBuffer_(new Buffer),
  thread_(std::bind(AsyncLogging::threadFun,this),"loggingThread")
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
    if(running_)
    {
        stop();
    }
}

void AsyncLogging::start()
{
    assert(!running_);
    running_=true;
    thread_->start();
    countDownLatch_.wait();
}
void AsyncLogging::stop()
{
    assert(running_);
    running_ = false;
    cond_.notify();//
    thread_.join();
}

void AsyncLogging::append(const char* logline,int len)
{
    MutexLockGuard lock(mutex_);
    if(currentBuffer_->avail()>=len)
    {
        currentBuffer_.append(logline,len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_)
        {
            currentBuffer_=std::move(nextBuffer_);
        }
        else
        {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_.append(logline,len);
        cond_.notify();
    }
}
void AsyncLogging::threadFunc()
{
    countDownLatch_.countDown();
    LogFile output(baseName_,rollSize_,flushInterval_,false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer1->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_)
    {
        assert(newBuffer1 && newBuffer1->length()==0);
        assert(newBuffer2 && newBuffer2->length()==0);
        aseert(buffersToWrite.size()==0);

        {
            MutexLockGuard lock(mutex_);
            if(buffers.empty())
            {
                cond_.waitForSeconds(flushInterval_);
            }
            buffers.push_back(std::move(currentBuffer_));
            currentBuffer_.reset(std::move(newBuffer1));
            if(!nextBuffer_)
            {
                nextBuffer_.reset(std::move(newBuffer2));
            }
            buffersToWrite.swap(buffers);
        }
        if(buffersToWrite.size() > 25)
        {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
                    Timestamp::now().toFormattedString().c_str(),buffersToWrite.size()-2);
            fputs(buf, stderr);
            output.append(buf,strlen(buf));
            buffersToWrite.earse(buffersToWrite.begin(),buffersToWrite.end()-2);
        }

        for(auto buffer:buffersToWrite)
        {
            output.append(buffer->data(),buffer->length());
        }
        if(buffersToWrite.size()>2)
        {
            buffersToWrite.resize(2);
        }
        
        if(!newBuffer1)
        {
            newBuffer1=std::move(buffersToWrite.back());
            newBuffer1->reset();
            buffersToWrite.pop_back();
        }
        if(!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2=std::move(buffersToWrite.back());
            newBuffer2->reset();
            buffersToWrite.pop_back();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}