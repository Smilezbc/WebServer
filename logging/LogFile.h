#ifndef WEBSERVER_LOGFILE_H
#define WEBSERVER_LOGFILE_H

#include<string>
#include<boost/noncopyable.hpp>
#include<boost/scoped_ptr.hpp>

namespace webServer
{

class MutexLock;

class LogFile
{
public:
    LogFile(const std::string& baseName,
            size_t rollSize,
            int flushInterval=3,
            bool threadSafe=false);
    ~LogFile();

    
    void append(const char* logline,int len);
    void flush();
private:
    
    void append_unlocked(const char* logline,int len);
    
    static std::string getLogFileName(const std::string& baseName,time_t* now);
    void rollFile();

    
    class File;

    const std::string baseName_;
    const size_t rollSize_; 
    const int flushInterval_;

    int count_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    
    boost::scoped_ptr<MutexLock> mutex_;
    boost::scoped_ptr<File> file_;

    const static int kCheckTimeRoll_=1024; //
    const static int kRollPerSeconds_ = 60*60*24;//seconds
};
}
#endif