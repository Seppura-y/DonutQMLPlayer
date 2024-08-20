#include "donut_av_timer.h"

#include <chrono>

extern"C"
{
    #include <libavutil/time.h>
}

namespace Donut
{
    long long DonutAVTimer::getTime()
    {
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> time_point =
            std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

        auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());

        return (long long)tmp.count();
    }

    DonutAVClock::DonutAVClock()
    {
        speed_ = 1.0;
    }

    void DonutAVClock::setClockAt(double pts, int serial, double time)
    {
        this->pts_ = pts;
        this->last_updated_ = time;
        this->pts_drift_ = this->pts_ - time;
        this->serial_ = serial;
    }

    double DonutAVClock::getClock()
    {
        //if (*queue_serial_ != serial_)
        //{
        //    return NAN;
        //}
        if (paused_)
        {
            return pts_;
        }
        else
        {
            double time = av_gettime_relative() / 1000000.0;
            return pts_drift_ + time - (time - last_updated_) * (1.0 - speed_);
        }
        return 0.0;
    }

}