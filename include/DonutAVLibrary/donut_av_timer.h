#ifndef DONUT_AV_TIMER_H
#define DONUT_AV_TIMER_H

#include "core.h"

namespace Donut{


class DONUT_API DonutAVTimer
{
public:
    static long long getTime();
};


class DONUT_API DonutAVClock
{
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int* queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
};

class DONUT_API DonutAVRational
{
public:
	DonutAVRational(int num, int den) :num_(num), den_(den) {}
	DonutAVRational(DonutAVRational& other) { num_ = other.num_; den_ = other.den_; }
	DonutAVRational& operator=(DonutAVRational& other) { num_ = other.num_; den_ = other.den_; }
	//private:
	int num_;
	int den_;
};
}


#endif