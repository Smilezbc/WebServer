#ifndef WEBSERVER_TIMESTAMP_H
#define WEBSERVER_TIMESTAMP_H
namespace webServer
{

class TimeStamp
{
public:
    static const int kMicroSecondsPerSecond=1000*1000;
    TimeStamp()
      :microSecondsSinceEpoch_(0)
    {
    }
    TimeStamp(int64_t microSecondsSinceEpoch)
     :microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {
    }
    ~TimeStamp()
    {
    }
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_;}
    static TimeStamp now();
    TimeStamp invalid(){return TimeStamp();}
    bool operator<(const TimeStamp& time);
    bool valid() const { return microSecondsSinceEpoch_ > 0; }
    
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