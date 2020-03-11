#include "TimeStamp.h"
#include <sys/time.h>

using namespace webServer;

TimeStamp TimeStamp::now()
{
    struct timeval tv;
    gettimeofday(&tv);
    return TimeStamp(tv.tv_sec*kMicroSecondsPerSecond + tv.tv_usec);
    
}
bool TimeStamp::operator<(const TimeStamp& time)
{
    return microSecondsSinceEpoch_<time.microSecondsSinceEpoch_;
}