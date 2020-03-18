#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H

#include<string>

namespace webServer
{

class HttpRequest
{
public:
    enum Method{ kGet, kPost, kHead, kPush, kDelete, kInvalid};
    enum Version{ kHttp10, kHttp11, kUnknown};
    HttpRequest(/* args */);
    ~HttpRequest();
    string getHead(const string& field);
    const Version version();
    const string& path();
    bool setMothed(const char* beg,const char* end);
    void setPath(const char* beg,const char* end){ path_ = std::string(beg,end); }
    void setQuery(const char* beg,const char* end){ query_ = std::string(beg,end); }
    void setVersion(Version version){ version_ = version; }
    void setReceiveTime(Timestamp& receiveTime){ receiveTime_=receiveTime; }
    void addHead(const char* beg,const char* colon,const char* end);
private:
    Method method_;
    std::string path_;
    std::string query_;
    Version version_;
    TimeStamp receiveTime_;
    std::map<string,string> heads_;
};

    
}
#endif