#include "Logger.h"


namespace webServer
{
__thread char* t_errnobuf[512];
__thread char* t_time[32];
__thread time_t t_lastSecond;

const char* logLevelName[]=
{
    "TRACE ",
    "DEBUG ",
    "INFO ",
    "WARN ",
    "ERROR ",
    "FATAL "
};
char* strerror_tl(int savedError)
{
    return strerror_r(savedError,t_errnobuf,sizeof t_errnobuf);
}

void defaultOutput(const char* buf,int len)
{
    fwrite(buf,1,len,stdout);
}

void defaultFlush()
{
    fflush(stdout);
}

Logger::LogLevel initLogLevel()
{
    if(::getenv("WEBSERVER_LOG_TRACE"))
    {
        return Logger::TRACE;
    }
    else
    {
        return Logger::DEBUG;
    }
}

Logger::LogLevel g_logLevel=initLogLevel();
Logger::outPutFunc g_output=defaultOutput;
Logger::flushFunc g_flush=defaultFlush;
    
}

using namespace webServer;

void Logger::Impl::formatTime()
{
    int64_t mircoSecondsSinceEpoch=time_.microSecondsSinceEpoch();
    time_t seconds=static_cast<time_t>(mircoSecondsSinceEpoch/kMicroSecondsPerSecond);
    int64_t microSeconds=mircoSecondsSinceEpoch%kMicroSecondsPerSecond;
    if(seconds!=t_lastSecond)
    {
        t_lastSecond=seconds;
        struct tm tm_;
        ::gmtime_r(&seconds,&tm_);
        //snprintf(t_time,sizeof t_time,"%04d%02d%02d %02d:%02d:%02d",tm_);
        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len==17);
    }
    Fmt fmt(".06dZ ",microSeconds);
    stream_<<T(t_time,17)<<fmt;
}
Logger::Impl(LogLevel level,int savedErrno,char* file,int line)
   :stream_(),
   time_(TimeStamp::now()),
   level_(level),
   fullName_(file),
   baseName_(nullptr),
   line_(line)
{
    char* pos_seq=strrchr(fullName_,'/');
    baseName_=pos_seq!=nullptr?pos_seq+1,fullName_;

    formatTime();
    // int tid=gettid();
    // stream_<<tid;
    Fmt tid("%5d ",CurrentThread::tid());
    stream_<<tid<<logLevelName[level_];
    if(savedErrno!=0)
    {
        //stream_<<strerror_tl(savedErrno);
        stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
    }
}
Logger(char*file,int line)
   :impl_(INFO,0,file,line);
{
}
Logger(LogLevel level,char*file,int line)
   :impl_(level,0,file,line);
{
}
Logger(LogLevel level,char*file,int line,char* func)
   :impl_(level,0,file,line);
{
    impl_.stream<<func<<' ';
}
Logger(char*file,int line,bool toAbort)
   :impl_(toAbort?FATAL:ERROR,errno,file,line);
{
}
void Logger::Impl::finish()
{
    stream_<<" - "<<basename_<<':'<<line_<<'\n';
}
Logger::~Logger()
{
    impl_.finish();
    LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(),buf.length());
    if(impl_.level_==FATAL)
    {
        g_flush();
        ::abort();
    }
}
Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}
void Logger::setLogLevel(LogLevel level)
{
    g_logLevel=level;
}
void Logger::setOutput(outPutFunc func)
{
    g_output=func;
}
void Logger::setFlush(flushFunc func)
{
    g_flush=func;
}