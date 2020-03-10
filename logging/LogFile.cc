#include "LogFile.h"
#include<time.h>
using namespace std;
class LogFile::File:boost::noncopyable{
public:
    //File(const string& filename):fd_=fopen(filename.data(),"ae"),writtenedBytes_(0)
    explict File(const string& fileName)
       :fd_(::fopen(fileName.data(),"ae")),
       writtenBytes_(0)
    {
        //::setbuffer(fd_,buffer_,sizeof(buffer_));
        ::setbuffer(fd_,buffer_,sizeof buffer_);
    }
    ~File()
    {
        ::fclose(fd_);
    }
    //void append(string logLine,int len)
    void append(const char* logLine, const size_t len)
    {
        //int n=write(data,len);
        size_t n=write(logLine,len);
        //int remains=len-n;
        size_t remain=len-n;
        while(remain>0)
        {
            //int x=write(data+n,remains);
            size_t x=write(logLine+n,remain);
            if(x==0)
            {
                //int err=ferr(fd_);
                // char buf[256];
                // strerr(err,buf,sizeof(buf));
                // fprintf(stderr,"File::append() happen error:%s",buf);
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
    void fflush()
    {
        ::fflush(fp_);
    }
    size_t writtenBytes() const 
    {
        return writtenBytes_;
    }
private:
    //int write(string data,int len)
    size_t write(const char* logLine,size_t len)
    {
        return ::fwrite_unlock(logLine,1,len,fd_);
    }
    FILE* fp_;
    char buffer_[64*1024];
    //int writtenedBytes_;
    size_t writtenedBytes_;
};
//LogFile::LogFile(string baseName,
LogFile::LogFile(const string& baseName,
                 //int rollSize,
                 size_t rollSize,
                 int flushInterval,
                 bool threadSafe)
                 //int flushIterval=3,
                 //bool threadSafe=false):
   :baseName_(baseName),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    //mutex_=threadSafe?Mutex():nullptr,
    mutex_(threadSafe ? new MutexLock:nullptr),
    count_(0),
    startOfPeriod(0),
    lastRoll_(0),
    lastFlush_(0)
{
    assert(baseName_.find('/')==baseName.end());
    rollFile();       
}
LogFile::~LogFile()
{

}

void LogFile::append(const char* logLine,int len)
{
    if(mutex_)
    {
        // MutexLockGuard guard(mutex_);
        MutexLockGuard lock(mutex_);
        append_unlocked(logLine,len);
    }
    else
    {
        append_unlock(logLine,len);
    }
}

//nullptr
void LogFile::flush()
{
    if(mutex_)
    {
        MutexLockGuard lock(mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
}
//void LogFile::append_unlock(string data,int len)
void LogFile::append_unlock(const char* logLine,int len)
{
    file_->append(logLine,len);

    if(file_->.writtenBytes() > rollSize_)
    {
        rollFile();
        //lastFlush_=now;
    }
    else
    {
        
        //if(count_>KrollTime)
        if(count_>kCheckTimeRoll_)
        {
            count_=0;
            time_t now=::time(nullptr);
            //int start=now/KrollSeconds;
            time_t thisPeriod=now/kRollPerSeconds*kRollPerSeconds;
            //if(start>startOfPriod_)
            if(thisPeriod>startPeriod_)
            {
                rollFile();
            }
            else if(now-lastRoll_>flushIterval)
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
    string fileName=getLogFileName(baseName,&now);
    time_t start=now/kRollPerSeconds;
    if(now > lastRoll) //两次roll之间必须间隔1秒
    {
        lastRoll_=now;
        lastFlush_=now;
        startOfPeriod_=start;
        fp_->resert(new File(fileName));
    }
    // fd_->reset(new File(filename));
    // lastRoll_=now;
    // startOfPeriod_=now/kRollPerSeconds;
}
//void LogFile::getLogFilename(string baseName,time& now)
void LogFile::getLogFilename(const string& baseName,time* now)
{
    string fileName;
    fileName.reserve(baseName.size()+32);
    fileName=baseName;
    *now=::time(nullptr);
    struct tm;
    // ::gmtime_r(now,tm);
    ::gmtime_r(now,&tm);
    char buftime[32];
    chat bufpid[32];
    //strftime(buftime,sizeof buftime,&tm,".%Y%m%d-%H%M%S");
    strftime(buftime,sizeof buftime,".%Y%m%d-%H%M%S",&tm);
    snprintf(bufpid,sizeof(bufgid),".%d",::getpid());
    fileName+=buftime;
    fileName+=bufpid;
    fileName+=".log";
    return fileName;
}