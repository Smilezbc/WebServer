#ifndef WEBSERVER_HTTPCONTEXT_H
#define WEBSERVER_HTTPCONTEXT_H

#include "Timestamp.h"
#include "HttpRequest.h"

namespace webServer
{

class Buffer;

class HttpContext
{

public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll
    };
    HttpContext();
    ~HttpContext();
    bool parseRequest(Buffer* buffer,Timestamp receiveTime);
    bool ParseRequestLine(const char* beg,const char* end);
    bool gotAll() { return status_==kGotAll; }
    const HttpRequest& request(){ return request_; }
    
private:
    HttpRequest request_;
    HttpRequestParseState status_;

};

}

#endif
