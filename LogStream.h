#ifndef WEBSERVER_LOGSTREAM_H
#define WEBSERVER_LOGSTREAM_H
#include<boost/noncopyable.hpp>
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
    FixedBuffer():cur(buffer_)
    {
        setCookie(cookieBegin);
    }
    ~FixedBuffer()
    {
        setCookie(cookieEnd);
    }
    void append(const char* buf,int len)
    {
        if(avil() >= len)
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
    void cookieBegin();
    void cookieEnd();

    void debugString();//这个的作用是什么
    string asString() const { return string(data_,length()); }

  private:
    char* end(){return data_+Size};
    char buffer_[Size];
    char* cur;
    void (*cookie_)();
}
}
struct T //这个类的作用是什么呢
{
    T(char* str,int len):str_(str),len_(len)
    {
        assert(len==strlen(buf));
    }
    char* str_;
    size_t len_;
}
class LogStream
{
public:
    typedef detail::FixedBuffer<detail::kSmallBufferSize> Buffer;
    typedef LogStream self;
    void LogStream::append(const char* buf,int len)
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

    self& operator<<(const string& s)
    {
        buffer_.append(s.c_str(),s.size());
        return *this;
    }
    
    self& operator<<(const T& s)
    {
        buffer_.append(T.str_,T.len_);
        return *this;
    }

    const Buffer& buffer()const {return buffer_; }
    void resetBuffer(){ buffer_.reset(); }
private:
    void staticCheck();
    void formatInteger(T val);
    Buffer buffer_;
    static const int kMaxNumericSize=32;
}

class Fmt //这个类的作用:将数值类型变量按用户自定义的方法格式化成字符串
{
  public:
    template<typename T>
    Fmt(const char* fmt,T val);
    const char* data() const { return data_;}
    size_t length() const { return len_};
  private:
    char* data_;
    int len_;
}
inline LogStream& operator<<(LogStream& s,const Fmt& fmt)
{
    s.append(fmt.data(),fmt.length());
    return s;
}
}
#endif