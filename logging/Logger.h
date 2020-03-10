#ifndef WEBSERVER_LOGGER_H
#define WEBSERVER_LOGGER_H
#include "LogStream.h"
#include "TimeStamp.h"
namespace webServer
{
class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };
    Logger(char* file,int line);
    Logger(LogLevel level,char* file,int line);
    Logger(LogLevel level,char* file,int line,char* func);
    Logger(char* file,int line,bool toAbort);

    LogStream& stream(){ return impl_.stream_;}

    typedef void (*outPutFunc)(const char* buf,int len);
    typedef void (*flushFunc)();
    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);
    static void setOutput(outPutFun func);
    static void setFlush(flushFun func);
private:
    class Impl
    {
    public:
        Impl(LogLevel level,int savedError,const char* file,int line);
        void formatTime();
        void finish();

        LogStream stream_;
        TimeStamp time_;
        LogLevel level;
        char* fullname;
        char* basename;
        int line;
    }
    Impl impl_;
};
#define LOG_TRACE webServer::Logger(webServer::Logger::TRACE,__FILE__,__LINE__,__func__).steram()
#define LOG_DEBUG webServer::Logger(webServer::Logger::DEBUG,__FILE__,__LINE__,__func__).steram()

#define LOG_INFO webServer::Logger(__FILE__,__LINE__).steram()

#define LOG_WARN webServer::Logger(webServer::Logger::WARN,__FILE__,__LINE__).steram()
#define LOG_ERROR webServer::Logger(webServer::Logger::ERROR,__FILE__,__LINE__).steram()
#define LOG_FATAL webServer::Logger(webServer::Logger::FATAL,__FILE__,__LINE__).steram()

#define LOG_SYSERR webServer::Logger(__FILE__,__LINE__,false).steram()
#define LOG_SYSFATAL webServer::Logger(__FILE__,__LINE__,true).steram()

const char* strerror_tl(int savedErrno);
}
#endif