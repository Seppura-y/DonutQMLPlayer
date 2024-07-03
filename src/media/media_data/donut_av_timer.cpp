#include "donut_av_timer.h"

#include <chrono>

namespace Donut
{
    long long DonutAVTimer::getTime()
    {
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> time_point =
            std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

        auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());

        return (long long)tmp.count();
    }

    void DonutAVClock::setClockAt(double pts, int serial, double time)
    {
        this->pts_ = pts;
        this->last_updated_ = time;
        this->pts_drift_ = this->pts_ - time;
        this->serial_ = serial;
    }

}