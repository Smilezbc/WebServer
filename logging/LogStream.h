#ifndef WEBSERVER_LOGSTREAM_H
#define WEBSERVER_LOGSTREAM_H

#include<boost/noncopyable.hpp>
#include<string>
#include<cstring>
#include<cassert>

namespace webServer
{

namespace detail
{

const int kSmallBufferSize=4000;
const int kLargeBufferSize=4000*1000;

template<int Size>
class FixedBuffer
{
  public:
    FixedBuffer():cur_(data_)
    {
        setCookie(cookieBegin);
    }
    ~FixedBuffer()
    {
        setCookie(cookieEnd);
    }
    void append(const char* buf,int len)
    {
        if(avail() >= len)
        {
            ::memcpy(cur_,buf,len);
            cur_+=len;
        }
    }
    const char* data() const{ return data_;}
    char* current() const{ return cur_;}
    int length() const {return cur_-data_;}
    void reset(){cur_=data_;}
    void add(size_t len){ cur_+=len; }
    int avail() const { return end()-cur_; }
    void setCookie(void (*cookie)()){ cookie_=cookie; }

    void debugString();//这个的作用是什么
    std::string asString() const { return std::string(data_,length()); }

  private:
    const char* end() const {return data_+Size;}
    char data_[Size];
    char* cur_;
    void (*cookie_)();
    static void cookieBegin();
    static void cookieEnd();
};

}

struct T 
{
    T(char* str,int len):str_(str),len_(len)
    {
        assert(len==strlen(str));
    }
    char* str_;
    size_t len_;
};

class LogStream
{
public:
    typedef detail::FixedBuffer<detail::kSmallBufferSize> Buffer;
    typedef LogStream self;
    void append(const char* buf,int len)
    {
        buffer_.append(buf,len);
    }
    self& operator<<(bool v)
    {
        buffer_.append(v?"1":"0",1);
        return *this;
    }
    self& operator<<(char v)
    {
        buffer_.append(&v,1);
        return *this;
    }
    template<typename T>
    void formatInteger(T val);
    self& operator<<(short v);
    self& operator<<(unsigned short v);
    self& operator<<(int v);
    self& operator<<(unsigned int v);
    self& operator<<(long v);
    self& operator<<(unsigned long v);
    self& operator<<(long long v);
    self& operator<<(unsigned long long v);
    
    self& operator<<(float v);
    self& operator<<(double v);
    self& operator<<(long double v);

    self& operator<<(void* p);

    self& operator<<(const std::string& s)
    {
        buffer_.append(s.c_str(),s.size());
        return *this;
    }
    
    self& operator<<(const T& t)
    {
        buffer_.append(t.str_, t.len_);
        return *this;
    }

    const Buffer& buffer()const {return buffer_; }
    void resetBuffer(){ buffer_.reset(); }
private:
    void staticCheck();
    void formatInteger(T val);
    Buffer buffer_;
    static const int kMaxNumericSize=32;
};

class Fmt //这个类的作用:将数值类型变量按用户自定义的方法格式化成字符串
{
  public:
    template<typename T>
    Fmt(const char* fmt,T val);
    const char* data() const { return data_;}
    size_t length() const { return len_;}
  private:
    char* data_;
    int len_;
};
inline LogStream& operator<<(LogStream& s,const Fmt& fmt)
{
    s.append(fmt.data(),fmt.length());
    return s;
}

}
#endif