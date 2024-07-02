#include "i_donut_video_view.h"

extern"C"
{
	#include <libavutil/avutil.h>
	#include <libavutil/time.h>
}

namespace Donut
{
	void IDonutVideoView::updateHandler(void* data)
	{
	}

	void IDonutVideoView::setClocks(std::shared_ptr<DonutAVClock>& a_clock, std::shared_ptr<DonutAVClock>& v_clock)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		audio_clock_ = a_clock;
		video_clock_ = v_clock;
	}

	void IDonutVideoView::updateVideoPts(double pts, int64_t pos, int serial)
	{
		double time = av_gettime_relative() / 1000000.0;
		video_clock_->pts_ = pts;
		video_clock_->last_updated_ = time;
		video_clock_->pts_drift_ = video_clock_->pts_ - time;
		video_clock_->serial_ = serial;
	}
}