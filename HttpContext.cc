#include "HttpContent.h"

using namespace webServer;

HttpContent::HttpContent()
    :status_(kExpectRequestLine)
{
}
HttpContent::~HttpContent()
{

}
bool HttpContent::parseRequestLine(const char* beg,const char* end)
{
    bool success=false;
    const char* start=beg;
    const char* space=std::find(start,end,' ');
    if(space!=end && request.setMethod(start,space))
    {
        start=space+1;
        space=std::find(start,end,' ');
        if(space!=end)
        {
            const char* question=std::find(start,space);
            if(question!=space)
            {
                request_.setPath(start,question);
                request_.setQuery(question+1,space);
            }
            else
            {
              request_.setPath(start,space);
            }
            start=space+1;
            string version(start,end);
            if(version=="HTTP/1.0")
            {
                success=true;
                request_.setVersion(kHttp10);
            }
            else if(version=="HTTP/1.1")
            {
                success=true;
                request_.setVersion(kHttp11);
            }
        }
    }
}
HttpContent::parseRequest(Buffer* buffer,TimeStamp receiveTime)
{
    bool ok=true;
    bool hasMore=true;
    while(hasMore)
    {
        if(status_==kExpectRequestLine)
        {
            const char* crlf=buffer.findCRLF();
            if(crlf)
            {
                ok=ParseRequestLine(buffer.peek(),crlf);
                if(ok)
                {
                    requeset_.setReceiveTime(time);
                    buffer->retrieveUntil(crlf+2);
                    status_=kExpectHead;
                }
                else
                {
                    hasMore=false;
                }
            }
            else
            {
                hasMore=false;
            }
        }
        else if(status_==kExpectHead)
        {
            const char* crlf=buffer_->findCRLF();
            if(crlf)
            {
                const char* colon=std::find(buffer->peek(),crlf,':');
                if(colon==crlf)//没有冒号
                {
                    status_=kGotAll;
                    hasMore=false;
                }
                else
                {
                    request_.addHead(buffer->peek(),colon,crlf);
                }
                buffer->retrieveUntil(crlf+2);
            }
            else
            {
                hasMore=false;
            }
        }
        else if(status_==kExpectBody)
        {

        }
    }
    return ok;
    
}
