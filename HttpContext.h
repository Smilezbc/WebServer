#ifndef WEBSERVER_HTTPCONTEXT_H
#define WEBSERVER_HTTPCONTEXT_H

#include "Buffer.h"
#include "TimeStamp.h"
#include "HttpRequest.h"

namespace webServer
{

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
    bool parseRequest(Buffer* buffer,TimeStamp receiveTime);
    bool ParseRequestLine(const char* beg,const char* end);
    bool gotAll() { return status_==kGotAll; }
private:
    HttpRequest request_;
    HttpRequestParseState state_;

};

}

#endif
