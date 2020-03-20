#include "Buffer.h"
#include "SocketOpts.h"

#include <errno.h>
#include <sys/uio.h>

using namespace webServer;

const int Buffer::kCheapPrepend=8;
const int Buffer::kInitialSize=1024;
const char Buffer::kCRLF[] = "\r\n";

size_t Buffer::readfd(int fd,int* saveErrno)
{
    char extrbuff[65536];
    struct iovec ivec[2];
    const size_t writable=writableBytes();
    ivec[0].iov_base=begin()+writeableIndex_;
    ivec[0].iov_len=writable;
    ivec[1].iov_base=extrbuff;
    ivec[1].iov_len=sizeof extrbuff;
    size_t n=readv(fd,ivec,2);
    if(n<0)
    {
        *saveErrno=errno;
    }
    else if(n>writable)
    {
        writeableIndex_=buffer_.size();
        append(extrbuff,n-writable);
    }else
    {
        writeableIndex_+=n;
    }
    return n;
}
const char* Buffer::findCRLF()
{
    const char* crlf=std::search(peek(),beginWrite(),kCRLF,kCRLF+2);
    if(crlf!=beginWrite())
    {
        return crlf;
    }
    else
    {
        return nullptr;
    }
}