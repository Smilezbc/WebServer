#include "HttpRequest.h"
using namespace webServer;

HttpRequest::HttpRequest()
{

}
HttpRequest::~HttpRequest()
{

}
bool HttpRequest::setMothed(const char* beg,const char*end)
{
    string method(beg,end);
    if(method=="GET")
    {
        method_=kGet;
    }
    else if(method_=="POST")
    {
        method_=kPost;
    }
    else if(method_=="PUSH")
    {
        method_=kPush;
    }
    else if(method_=="DELETE")
    {
        method_=kDelete;
    }
    else if(method_=="HEAD")
    {
        method_=kHead;
    }
    else
    {
        method_=kInvalid;
    }
    return method_!=kInvalid;
    
}
void HttpRequest::addHead(const char* beg,const char* colon,const char* end)
{
    string field(beg,colon);
    ++colon;
    while(colon<end && *colon==' ') ++colon;
    string value(colon,end);
    while(!value.empty() && value[value.size()-1]==' ')
    {
        value.resize(value.size()-1);
    }
}
string HttpRequest::getHead(const string& field)
{
    string value;
    std::map<string,string>::iterator it=heads_.find(field);
    if(it!=heads_.end())
    {
        value=it->second;
    }
    return value;
}