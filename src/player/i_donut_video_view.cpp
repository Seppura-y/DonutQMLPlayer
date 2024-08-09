#include "i_donut_video_view.h"

//extern"C"
//{
//    #include <libavcodec/avcodec.h>
//    #include <libavutil/rational.h>
//	#include <libavutil/avutil.h>
//	#include <libavutil/time.h>
//}

namespace Donut
{
	void IDonutVideoView::updateHandler(void* data)
	{
	}

	void IDonutVideoView::setClocks(std::shared_ptr<DonutAVClock>& m_clock, std::shared_ptr<DonutAVClock>& clock)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		master_clock_ = m_clock;
        master_timebase_ = m_clock->timebase_;

		clock_ = clock;
        timebase_ = clock->timebase_;
    }

	void IDonutVideoView::setFrameQueue(std::shared_ptr<DonutAVFrameQueue>& queue)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		video_frame_queue_ = queue;
	}

	void IDonutVideoView::updateVideoPts(double pts, int64_t pos, int serial)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		double time = av_gettime_relative() / 1000000.0;
		clock_->pts_ = pts;
		clock_->last_updated_ = time;
		clock_->pts_drift_ = clock_->pts_ - time;
		clock_->serial_ = serial;
	}

    double IDonutVideoView::getFrameDiffTime(AVFrame* frame)
    {
        if (!master_clock_ || !clock_) return 0;

        if (master_clock_ == clock_)
        {
            is_need_sync_ = false;
            return 0;
        }
        else
        {
            is_need_sync_ = true;
        }

        double pts = frame->pts;
        if (pts == AV_NOPTS_VALUE)
        {
            pts = 0;
        }

        pts *= av_q2d(timebase_);
        if (pts > 0)
        {
            clock_->setClockAt(pts, 0, pts);
        }

        double diff = master_clock_->pts_ - clock_->pts_;

        return diff;
    }

    double IDonutVideoView::getDelayTime(double diff)
    {
        double delay_time = 0;
        double default_delay_time = 0.04;
        if (diff > 0.003)
        {
            // master超前，调整睡眠时间为2/3
            delay_time = delay_time * 2 / 3;
            if (delay_time < default_delay_time / 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2/3
                delay_time = default_delay_time * 2 / 3;
            }
            else if (delay_time > default_delay_time * 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2倍
                delay_time = default_delay_time * 2;
            }
        }
        else if (diff < -0.003)
        {
            // master落后，调整为delay time的1.5倍
            delay_time = delay_time * 3 / 2;
            if (delay_time < default_delay_time / 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2/3
                delay_time = default_delay_time * 2 / 3;
            }
            else if (delay_time > default_delay_time * 2)
            {
                // 睡眠时间最少是默认值(40ms)的 2倍
                delay_time = default_delay_time * 2;
            }
        }

        // master超前500ms
        if (diff >= 0.5)
        {
            // 直接跳过本帧
            delay_time = 0;
        }
        else if (diff <= -0.5)
        {
            // master落后超过500ms，2倍默认值(2 * 40ms)
            delay_time = default_delay_time * 2;
        }

        // master超前10秒，不进行同步，直接清空slave队列
        if (diff >= 10)
        {
            //packet_queue_->packetQueueFlush();
            delay_time = default_delay_time;
        }

        // master落后10秒，不进行同步，直接清空master队列
        if (diff <= -10)
        {
            //packet_queue_->packetQueueFlush();
            delay_time = default_delay_time;
        }

        return delay_time;
    }
}