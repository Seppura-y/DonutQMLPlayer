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
            // master��ǰ������˯��ʱ��Ϊ2/3
            delay_time = delay_time * 2 / 3;
            if (delay_time < default_delay_time / 2)
            {
                // ˯��ʱ��������Ĭ��ֵ(40ms)�� 2/3
                delay_time = default_delay_time * 2 / 3;
            }
            else if (delay_time > default_delay_time * 2)
            {
                // ˯��ʱ��������Ĭ��ֵ(40ms)�� 2��
                delay_time = default_delay_time * 2;
            }
        }
        else if (diff < -0.003)
        {
            // master��󣬵���Ϊdelay time��1.5��
            delay_time = delay_time * 3 / 2;
            if (delay_time < default_delay_time / 2)
            {
                // ˯��ʱ��������Ĭ��ֵ(40ms)�� 2/3
                delay_time = default_delay_time * 2 / 3;
            }
            else if (delay_time > default_delay_time * 2)
            {
                // ˯��ʱ��������Ĭ��ֵ(40ms)�� 2��
                delay_time = default_delay_time * 2;
            }
        }

        // master��ǰ500ms
        if (diff >= 0.5)
        {
            // ֱ��������֡
            delay_time = 0;
        }
        else if (diff <= -0.5)
        {
            // master��󳬹�500ms��2��Ĭ��ֵ(2 * 40ms)
            delay_time = default_delay_time * 2;
        }

        // master��ǰ10�룬������ͬ����ֱ�����slave����
        if (diff >= 10)
        {
            //packet_queue_->packetQueueFlush();
            delay_time = default_delay_time;
        }

        // master���10�룬������ͬ����ֱ�����master����
        if (diff <= -10)
        {
            //packet_queue_->packetQueueFlush();
            delay_time = default_delay_time;
        }

        return delay_time;
    }
}