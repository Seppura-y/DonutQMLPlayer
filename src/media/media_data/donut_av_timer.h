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
public:
    double pts_;           /* clock base */
    double pts_drift_;     /* clock base minus time at which we updated the clock */
    double last_updated_;
    double speed_;
    int serial_;           /* clock is based on a packet with this serial */
    int paused_;
    int* queue_serial_;    /* pointer to the current packet queue serial, used for obsolete clock detection */

public:
    void setClockAt(double pts, int serial, double time);
};

class DONUT_API DonutAVRational
{
public:
    DonutAVRational() = default;
	DonutAVRational(int num, int den) :num_(num), den_(den) {}
	DonutAVRational(DonutAVRational& other) { num_ = other.num_; den_ = other.den_; }
	DonutAVRational& operator=(DonutAVRational& other) { num_ = other.num_; den_ = other.den_; }
	//private:
	int num_ = 0;
	int den_ = 0;
};
}


#endif