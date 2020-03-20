#ifndef WEBSERVER_LOGFILE_H
#define WEBSERVER_LOGFILE_H

#include<string>
#include<boost/noncopyable.hpp>
#include<memory>

namespace webServer
{

class MutexLock;

class LogFile:boost::noncopyable
{
public:
    LogFile(const std::string& baseName,
            const size_t rollSize,
            const int flushInterval=3,
            bool threadSafe=false);
    ~LogFile();

    
    void append(const char* logline,int len);
    void flush();
private:
    
    void append_unlocked(const char* logline,int len);
    
    std::string getLogFileName(const std::string& baseName,time_t* now);
    void rollFile();

    
    class File;

    const std::string baseName_;
    const size_t rollSize_; 
    const int flushInterval_;

    int count_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<File> file_;

    static const int kCheckTimeRoll_=1024; //
    static const int kRollPerSeconds_ = 60*60*24;//seconds
};
}
#endif