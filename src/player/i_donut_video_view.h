#ifndef I_DONUT_VIDEO_VIEW_H
#define I_DONUT_VIDEO_VIEW_H

#include "i_donut_av_base_handler.h"
#include "donut_av_timer.h"

namespace Donut
{
	class IDonutVideoView : public IDonutAVBaseHandler
	{
	public:
		virtual void updateHandler(void* data) override;
		void setClocks(std::shared_ptr<DonutAVClock>& a_clock, std::shared_ptr<DonutAVClock>& v_clock);


	protected:
		void updateVideoPts(double pts, int64_t pos, int serial);
	protected:
		std::shared_ptr<DonutAVClock> audio_clock_;
		std::shared_ptr<DonutAVClock> video_clock_;
	};
}
#endif