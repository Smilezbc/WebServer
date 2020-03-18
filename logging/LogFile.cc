#include "LogFile.h"
#include "thread/Mutex.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace std;
using namespace webServer;

class LogFile::File : boost::noncopyable 
{
public:
    
    explicit File(const string& fileName)
       :fp_(::fopen(fileName.data(),"ae")),
       writtenBytes_(0)
    {
        ::setbuffer(fp_,buffer_,sizeof buffer_);
    }
    ~File()
    {
        ::fclose(fp_);
    }
    
    void append(const char* logLine, const size_t len)
    {
        
        size_t n=write(logLine,len);
        
        size_t remain=len-n;
        while(remain>0)
        {
            size_t x=write(logLine+n,remain);
            if(x==0)
            {
                int err=ferror(fp_);
                if(err)
                {
                    char buf[128];
                    strerror_r(err,buf,sizeof buf);
                    fprintf(stderr,"LogFile::File::append() failed %s\n", buf);
                }
                break;
            }
            remain-=x;
            n+=x;
        }
        writtenBytes_+=len;
    }
    void flush()
    {
        ::fflush(fp_);
    }
    size_t writtenBytes() const 
    {
        return writtenBytes_;
    }
private:

    size_t write(const char* logLine,size_t len)
    {
        return ::fwrite_unlocked(logLine,1,len,fp_);
    }
    FILE* fp_;
    char buffer_[64*1024];
    size_t writtenBytes_;
};

LogFile::LogFile(const string& baseName,
                 size_t rollSize,
                 int flushInterval,
                 bool threadSafe)
   :baseName_(baseName),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    mutex_(threadSafe ? new MutexLock:nullptr),
    count_(0),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0)
{
    assert(baseName_.find('/')==string::npos);
    rollFile();       
}
LogFile::~LogFile()
{

}

void LogFile::append(const char* logLine,int len)
{
    if(mutex_)
    {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logLine,len);
    }
    else
    {
        append_unlocked(logLine,len);
    }
}

//nullptr
void LogFile::flush()
{
    if(mutex_)
    {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
}

void LogFile::append_unlocked(const char* logLine,int len)
{
    file_->append(logLine,len);

    if(file_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    else
    {
        if(count_>kCheckTimeRoll_)
        {
            count_=0;
            time_t now=::time(nullptr);
            time_t thisPeriod=now/kRollPerSeconds_*kRollPerSeconds_;
            if(thisPeriod>startOfPeriod_)
            {
                rollFile();
            }
            else if(now-lastRoll_>flushInterval_)
            {
                lastFlush_=now;
                file_->flush();
            }
        }
        else 
        {
            ++count_;
        }
    }
    
}
void LogFile::rollFile()
{
    time_t now=0;
    string fileName=getLogFileName(baseName_,&now);
    time_t start=now/kRollPerSeconds_;
    if(now > lastRoll_) //两次roll之间必须间隔1秒
    {
        lastRoll_=now;
        lastFlush_=now;
        startOfPeriod_=start;
        file_.reset(new File(fileName));
    }
}

string LogFile::getLogFileName(const string& baseName,time_t* now)
{
    string fileName;
    fileName.reserve(baseName.size()+32);
    fileName=baseName;
    *now=::time(nullptr);
    struct tm tm_;
    ::gmtime_r(now,&tm_);
    char buftime[32];
    char bufpid[32];
    strftime(buftime,sizeof buftime,".%Y%m%d-%H%M%S",&tm_);
    snprintf(bufpid,sizeof(bufpid),".%d",::getpid());
    fileName+=buftime;
    fileName+=bufpid;
    fileName+=".log";
    return fileName;
}