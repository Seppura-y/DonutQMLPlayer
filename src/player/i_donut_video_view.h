#ifndef I_DONUT_VIDEO_VIEW_H
#define I_DONUT_VIDEO_VIEW_H

#include "i_donut_av_base_handler.h"
#include "donut_av_timer.h"
#include "donut_av_frame.h"
extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/rational.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
}


namespace Donut
{
	class IDonutVideoView : public IDonutAVBaseHandler
	{
	public:
		virtual void updateHandler(void* data) override;
		void setClocks(std::shared_ptr<DonutAVClock>& m_clock, std::shared_ptr<DonutAVClock>& clock);

		void setFrameQueue(std::shared_ptr<DonutAVFrameQueue>& queue);

	protected:
		double getFrameDiffTime(AVFrame* frame);
		double getDelayTime(double diff);

		void updateVideoPts(double pts, int64_t pos, int serial);
	protected:
		std::shared_ptr<DonutAVClock> master_clock_;
		std::shared_ptr<DonutAVClock> clock_;
		std::shared_ptr<DonutAVFrameQueue> video_frame_queue_;

		int delay_time_ = 1;
		bool is_need_sync_ = true;

		AVRational master_timebase_{ 1, 1 };
		AVRational timebase_{ 1, 1 };
	};
}
#endif