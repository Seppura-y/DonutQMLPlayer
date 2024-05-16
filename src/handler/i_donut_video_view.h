#ifndef I_DONUT_VIDEO_VIEW_H
#define I_DONUT_VIDEO_VIEW_H

#include "i_donut_av_base_handler.h"

namespace Donut
{
	class IDonutVideoView : public IDonutAVBaseHandler
	{
	public:
		virtual void updateHandler(void* data) override;

	};
}
#endif