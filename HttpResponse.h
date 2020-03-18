#ifndef WEBSERVER_HTTPRESPONSE_H
#define WEBSERVER_HTTPRESPONSE_H

#include<string>
#include<map>

namespace webServer
{

class Buffer;

class HttpResponse
{
public:
    enum StatusCode
    {
        kUnknown,
        k200Ok=200,
        k301MovedPermanently=301,
        k400BadRequest=400,
        k404NotFound=404,
    };
    HttpResponse(bool close);
    ~HttpResponse();
    void appendToBuffer(Buffer* buffer);
    bool closeConnection() { return closeConnection_; }
    void setStatusCode(StatusCode statusCode){ statusCode_=statusCode; }
    void setStatusMessage(const string& statusMessage) { statusMessage_=statusMessage; }
    void setBody(const string& body) { body_=body; }
    void setCloseConnection(bool closeConnection) { closeConnection_=closeConnection; }
    void setContentType(const string& contentType) { addHead("Content-Type",contentType); }
    void addHead(const string& field,const string& value) { heads_[field] = value; }
private:
    StatusCode statusCode_;
    std::string statusMessage_;
    std::map<string,string> heads_;
    bool closeConnection_;
    string body_;
};
    
}
#endif