#ifndef I_DONUT_VIDEO_VIEW_H
#define I_DONUT_VIDEO_VIEW_H

#include "i_donut_av_base_handler.h"
#include "donut_av_timer.h"
#include "donut_av_frame.h"

//#include "donut_qml_av_manager.h"

extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/rational.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
}


namespace Donut
{
	class DonutQMLAVManager;
	class IDonutVideoView : public IDonutAVBaseHandler
	{
	public:
		virtual void updateHandler(void* data) override;
		void setClocks(std::shared_ptr<DonutAVClock>& m_clock, std::shared_ptr<DonutAVClock>& clock);

		void setFrameQueue(std::shared_ptr<DonutAVFrameQueue>& queue);
		void setManager(DonutQMLAVManager* manager);
		void setPaused(bool pause);


		virtual void reset() = 0;


		void setRotationX(float value);
		void setRotationY(float value);
		void setRotationZ(float value);
	protected:
		double getFrameDiffTime(AVFrame* frame);
		double getDelayTime(double diff);

		void updateVideoPts(double pts, int64_t pos, int serial);

		double computeTargetDelay(double delay);
		double computeDuration(std::shared_ptr<DonutAVFrame> vp, std::shared_ptr<DonutAVFrame> last_vp);
	protected:
		DonutQMLAVManager* manager_;

		std::shared_ptr<DonutAVClock> master_clock_;
		std::shared_ptr<DonutAVClock> clock_;
		std::shared_ptr<DonutAVFrameQueue> video_frame_queue_;

		int delay_time_ = 1;
		bool is_need_sync_ = true;
		double frame_timer_ = 0;

		AVRational master_timebase_{ 1, 1 };
		AVRational timebase_{ 1, 1 };

		bool is_resetting_ = false;


		float angle_x_ = 0;
		float angle_y_ = 0;
		float angle_z_ = 0;
	};
}
#endif