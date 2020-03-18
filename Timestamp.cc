#include "Timestamp.h"
#include <sys/time.h>

using namespace webServer;
Timestamp::Timestamp()
  :microSecondsSinceEpoch_(0)
{
}
Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
  :microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
}
Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,nullptr);
    return Timestamp(tv.tv_sec*kMicroSecondsPerSecond + tv.tv_usec);
    
}
bool Timestamp::operator<(const Timestamp& time)
{
    return microSecondsSinceEpoch_<time.microSecondsSinceEpoch_;
}