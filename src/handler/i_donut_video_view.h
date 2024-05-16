#ifndef I_DONUT_VIDEO_VIEW_H
#define I_DONUT_VIDEO_VIEW_H

#include "i_donut_av_base_handler.h"

class IDonutVideoView : public IDonutAVBaseHandler
{
public:
	virtual void update(void* data) override;

};

#endif