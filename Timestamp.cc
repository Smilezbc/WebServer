#include "Timestamp.h"

#include <sys/time.h>
#include<string>
#include <inttypes.h>

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

std::string Timestamp::toString() const
{
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}