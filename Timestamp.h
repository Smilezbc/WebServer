#ifndef WEBSERVER_TIMESTAMP_H
#define WEBSERVER_TIMESTAMP_H

#include <stdint.h>
#include <string>

namespace webServer
{

class Timestamp
{
public:
    static const int kMicroSecondsPerSecond=1000*1000;
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    ~Timestamp()
    {
    }
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_;}
    bool operator<(const Timestamp& time);
    bool valid() const { return microSecondsSinceEpoch_ > 0; }
    std::string toString() const;
    
    static Timestamp now();
    static Timestamp invalid(){return Timestamp();}
    
private:
    //time_t microSecondsSinceEpoch_;
    int64_t microSecondsSinceEpoch_;
};

inline Timestamp addTime(Timestamp timestamp,double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp( timestamp.microSecondsSinceEpoch() + delta);
}
inline double timeDifference(Timestamp high, Timestamp low)
{
  int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

}
#endif