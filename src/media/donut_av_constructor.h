#ifndef DONUT_AV_CONSTRUCTOR_H
#define DONUT_AV_CONSTRUCTOR_H

#include "i_donut_av_manager.h"
#include "donut_av_demux_handler.h"
#include "donut_av_decode_handler.h"
#include "donut_av_audio_player.h"

#include "i_donut_video_view.h"

#include "media_type.h"
namespace Donut
{
	class DonutAVConstructor
	{
	public:
		static IDonutAVManager* constructPlayer(DonuAVManagerType type = DonuAVManagerType::DONUT_AV_MANAGER_TYPE_QML);

	protected:
		DonutAVDemuxHandler* createDemuxer();
		DonutAVDecodeHandler* createDecoder();
		IDonutVideoView* createVideoView();
	};
}

#endif