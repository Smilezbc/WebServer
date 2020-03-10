#ifndef WEBSERVER_TIMESTAMP_H
#define WEBSERVER_TIMESTAMP_H
namespace webServer
{

class TimeStamp
{
public:
    static const int kMicroSecondsPerSecond=1000*1000;
    TimeStamp(int64_t microSecondsSinceEpoch)
     :microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {
    }
    
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_;}
    static TimeStamp now();
private:
    //time_t microSecondsSinceEpoch_;
    int64_t microSecondsSinceEpoch_;
};
inline TimeStamp addTime(TimeStamp timeStamp,double seconds)
{
    uint64_t delta = static_cast<uint64_t>(seconds*TimeStamp::kMicroSecondsPerSecond);
    return TimeStamp(timeStamp.microSecondsSinceEpoch()+delta);
}
}
#endif