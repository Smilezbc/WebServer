#ifndef WEBSERVER_LOGFILE_H
#define WEBSERVER_LOGFILE_H
#include<boost/noncopyable>
#include<string>
#include<boost/noncopyable.hpp>
#include<boost/scoped_ptr.hpp>

namespace WebServer
{
    using std::string
class LogFile: boost::noncopyable
{
public:
    //LogFile(string baseName,int rollSize,int flushInterval,bool threadSafe);
    LogFile(const string& baseName,
    size_t rollSize,
    int flushInterval=3,
    bool threadSafe=false);
    // ~LogFile()
    // {
    // }
    ~LogFile();

    //void append(string data,int len);
    void append(const char* logline,int len);
    void flush();
private:
    // void append_unlock();
    void append_unlocked(const char* logline,int len);
    //void getFileName();
    static string getLogFileName(const string& baseName,time_t* now);
    void rollFile();

    
    //string baseName_;
    const string baseName_;
    //int rollSize_;
    size_t rollSize; //字节数
    //int flushIterval;
    const int flushInterval;

    int count_;

    //MutexLock mutex_;
    boost::scoped_ptr<MutexLock> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    //scopted_str<File> file_;
    class File;
    boost::scopted_str<File> file_;

    const static int kCheckTimeRoll_=1024; //
    const static int kRollPerSeconds_ = 60*60*24;//seconds
};
}
#endif