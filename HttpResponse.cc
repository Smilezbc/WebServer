#include "HttpResponse.h"
#include "Buffer.h"

using namespace webServer;

HttpResponse::HttpResponse(bool close)
  :closeConnection_(close)
{

}
HttpResponse::~HttpResponse()
{

}
void HttpResponse::appendToBuffer(Buffer* buffer)
{
    char buf[32];
    snprintf(buf,sizeof buf,"HTTP/1.1 %d ",statusCode_);
    buffer->append(buf);
    buffer->append(statusMessage_);
    buffer->append("\r\n");

    if(closeConnection_)
    {
        buffer->append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf,sizeof buf,"Content-Length: %zd\r\n", body_.size());
        buffer->append(buf);
        buffer->append("Connection: Keep-Alive\r\n");
    }
    

    for(const auto& head:heads_)
    {
        buffer->append(ele.first+": "+ele.second+"\r\n");
    }
    buffer->append("\r\n");
    buffer->append(body_);
}