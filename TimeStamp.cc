#include "TimeStamp.h"
#include <sys/time.h>

using namespace webServer;

TimeStamp::TimeStamp(int64_t microSeconds)
  :microSecondsSinceEpoch_(microSeconds)
{

}

TimeStamp TimeStamp::now()
{
    struct timeval tv;
    gettimeofday(&tv);
    return TimeStamp(tv.tv_sec*kMicroSecondsPerSecond + tv.tv_usec);
    
}