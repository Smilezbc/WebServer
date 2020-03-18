#ifndef WEBSERVER_LOGGER_H
#define WEBSERVER_LOGGER_H

#include "LogStream.h"
#include "../Timestamp.h"

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
    Logger(const char* file,int line);
    Logger(const char* file,int line,LogLevel level);
    Logger(const char* file,int line,LogLevel level,const char* func);
    Logger(const char* file,int line,bool toAbort);

    LogStream& stream(){ return impl_.stream_;}

    typedef void (*outPutFunc)(const char* buf,int len);
    typedef void (*flushFunc)();
    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);
    static void setOutput(outPutFunc func);
    static void setFlush(flushFunc func);
private:
    class Impl
    {
    public:
        Impl(LogLevel level,int savedError,const char* file,int line);
        void formatTime();
        void finish();

        LogStream stream_;
        Timestamp time_;
        LogLevel level_;
        const char* fullName_;
        const char* baseName_;
        int line_;
    };
    Impl impl_;
    static const int kMicroSecondsPerSecond=1000*1000;
};

#define LOG_TRACE webServer::Logger(__FILE__,__LINE__,webServer::Logger::TRACE,__func__).stream()
#define LOG_DEBUG webServer::Logger(__FILE__,__LINE__,webServer::Logger::DEBUG,__func__).stream()

#define LOG_INFO webServer::Logger(__FILE__,__LINE__).stream()

#define LOG_WARN webServer::Logger(__FILE__,__LINE__,webServer::Logger::WARN).stream()
#define LOG_ERROR webServer::Logger(__FILE__,__LINE__,webServer::Logger::ERROR).stream()
#define LOG_FATAL webServer::Logger(__FILE__,__LINE__,webServer::Logger::FATAL).stream()

#define LOG_SYSERR webServer::Logger(__FILE__,__LINE__,false).stream()
#define LOG_SYSFATAL webServer::Logger(__FILE__,__LINE__,true).stream()

const char* strerror_tl(int savedErrno);
}

#endif