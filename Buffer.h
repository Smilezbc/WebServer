#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H

#include <algorithm>
#include <string>
#include <vector>

#include <assert.h>

namespace webServer
{

class Buffer
{
public:
    Buffer()
      :buffer_(kCheapPrepend+kInitialSize),
      readableIndex_(kCheapPrepend),
      writeableIndex_(kCheapPrepend)
    {
        assert(readableBytes()==0);
        assert(writableBytes()==kInitialSize);
        assert(prependBytes()==kCheapPrepend);
    }
    void swap(Buffer& rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readableIndex_,rhs.readableIndex_);
        std::swap(writeableIndex_,rhs.writeableIndex_);

    }
    size_t writableBytes() const {return buffer_.size()-writeableIndex_;}
    size_t readableBytes() const {return writeableIndex_-readableIndex_;}
    size_t prependBytes() const {return readableIndex_;}
    const char* peek(){return begin()+readableIndex_;}
    const char* beginWrite(){return begin()+writeableIndex_;}
    void retrieve(size_t len)
    {
        assert(readableBytes()>=len);
        readableIndex_+=len;
    }
    void retrieveUntil(char *end)
    {
        assert(peek()<=end);
        assert(beginWrite()>=end);
        retrieve(end-peek());
    }
    void retrieveAll()
    {
        readableIndex_=kCheapPrepend;
        writeableIndex_=kCheapPrepend;
    }
    std::string retrieveAsString()
    {
        std::string res(peek(),beginWrite());
        retrieveAll();
    }
    void ensureWritableBytes(size_t len)
    {
        if(writableBytes()<len)
        {
            makeSpec(len);
        }
        assert(writableBytes()>=len);
    }
    void haveWritten(size_t len)
    {
        writeableIndex_+=len;
    }
    void append(const char* data,size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data,data+len,beginWrite());
        haveWritten(len);
    }
    void append(void* data,size_t len)
    {
        char* ch=static_cast<char*>(data);
        append(ch,len);
    }
    void append(const std::string&s)
    {
        append(s.data(),s.size());
    }
    void prepend(void* data,size_t len)
    {
        assert(prependBytes()>=len);
        readableIndex_-=len;
        const char* d=static_cast<char*>(data);
        std::copy(d,d+len,begin()+readableIndex_);
    }
    void shrink(size_t reserve)//只保留reserve的剩余空间
    {
        int readable=readableBytes();
        std::vector<char> rhs(kCheapPrepend+readableBytes()+reserve);
        std::copy(peek(),beginWrite(),rhs.begin()+kCheapPrepend);
        buffer_.swap(rhs);
        readableIndex_=kCheapPrepend;
        writeableIndex_=kCheapPrepend+readable;
    }
    const char* findCRLF();
    size_t readfd(int fd,int*savedErrno);

private:
    char* begin(){return &*buffer_.begin();}
    const char* begin() const {return &*buffer_.begin();}
    void makeSpec(size_t len)
    {
        if(prependBytes()+writableBytes()<len+kCheapPrepend)
        {
            buffer_.resize(writeableIndex_+len);
        }
        else
        {
            int readable=readableBytes();
            std::copy(begin()+readableIndex_,begin()+writeableIndex_,begin()+kCheapPrepend);
            readableIndex_=kCheapPrepend;
            writeableIndex_=kCheapPrepend+readable;
        }
    }
    std::vector<char> buffer_;
    int readableIndex_;
    int writeableIndex_;
    static const int kCheapPrepend;
    static const int kInitialSize;
    static const char kCRLF[];
};

}

#endif