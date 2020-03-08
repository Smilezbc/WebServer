#include "LogStream.h"
using namespace webServer;
using namespace webServer::detail;
namespace webServer
{
namespaec detail
{
const char digits[]="9876543210123456789";
const char* zero=digits+9;
const char digitsHex[]="0123456789abcdef";
template<typename T>
int convert(char *buf,T val)
{
    T i=val;
    char *p=buf;
    do
    {
        int slab=i%10;
        i/=10;
        *p++=zero[slab];
    } while (i!=0);
    if(val<0)
    {
        *p++='-';
    }
    *p='\0';
    std::reverse(buf,p);
    return p-buf;
}
int convertHex(char *buf,uintptr_t val)
{
    uintptr_t i=val;
    buf[0]='0';
    buf[1]='x';
    char *p=buf+2;
    do
    {
        /* code */
        int slab=i%16;
        i/=16;
        *p++=buf[slab];
    } while (i!=0);
    *p='\0';
    std::reverse(buf,p);
    return p-buf;
}

}
}

//限制 模板类FixBuffer只能实例化成下面两种尺寸
template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;
template<int Size>
void FixedBuffer::cookieBegin()
{

}

template<int Size>
void FixedBuffer::cookieEnd()
{

}
template<int Size>
void FixedBuffer::debugString()
{
    *cur_='\0';
}


void LogStream::staticCheck()
{
    BOOST_STATIC_ASSERT(kMaxNumericSize-10 > std::numeric_limits<double>::digits10)
    BOOST_STATIC_ASSERT(kMaxNumericSize-10 > std::numeric_limits<long double>::digits10)
    BOOST_STATIC_ASSERT(kMaxNumericSize-10 > std::numeric_limits<long>::digits10)
    BOOST_STATIC_ASSERT(kMaxNumericSize-10 > std::numeric_limits<long long>::digits10)
}

template<typename T>
void formatInteger(T val)
{
    if(buffer_.avil()>=kMaxNumericSize)
    {
        int len=convert(buffer_.current(),val);
        buffer_.add(len);
    }
}
self& LogStream::operator<<(short val)
{
    *this<<static_cast<int>(val);
    return *this;
}
self& LogStream::operator<<(unsigned short val)
{
    *this<<static_cast<unsigned int>(val);
    return *this;
}
self& LogStream::operator<<(int val)
{
    formatInteger(val);
    return *this;
}
self& LogStream::operator<<(unsigned int val)
{
    formatInteger(val);
    return *this;
}
self& LogStream::operator<<(long val)
{
    formatInteger(val);
    return *this;
}
self& LogStream::operator<<(unsigned long val)
{
    formatInteger(val);
    return *this;
}
self& LogStream::operator<<(long long  val)
{
    formatInteger(val);
    return *this;
}
self& LogStream::operator<<(unsigned long long val)
{
    formatInteger(val);
    return *this;
}
self& LogStream::operator<<(float val)
{
    (*this)<<static_cast<double>(val);
    return *this;
}
self& LogStream::operator<<(double val)
{
    
    if(buffer_.avil()>=kMaxNumericSize)
    {
        int len=snprintf(buffer_.current,kMaxNumericSize,"%.12g",val);
        buffer_.add(len);
    }
    return &this;
}
self& LogStream::operator<<(long double val)
{
    
    if(buffer_.avil()>=kMaxNumericSize)
    {
        int len=snprintf(buffer_.current,kMaxNumericSize,"%.12g",val);
        buffer_.add(len);
    }
    return &this;
}
self& LogStream::operator<<(const void* p)
{
    uintptr_t val = reinterpret_cast<uintptr_t>(p);
    char* buf=buffer_.current();
    buf[0]='0';
    buf[1]='x';
    int len=convertHex(buf+2,val);
    buffer_.add(len+2);
}

template<typename T>
Fmt::Fmt(const char* fmt,T val)
{
    BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);
    len_ = snprintf(data_,sizeof data_,fmt,val);
    assert(static_cast<size_t>(len_)<sizeof data);
}

//限制Fmt的模板构造函数只能实例化成以下几种类型
template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);
template Fmt::Fmt(const char* fmt, long double);