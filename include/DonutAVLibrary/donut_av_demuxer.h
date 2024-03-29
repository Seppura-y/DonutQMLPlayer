#ifndef DONUT_AV_DEMUXER_H
#define DONUT_AV_DEMUXER_H

#include "core.h"
#include "donut_av_format_base.h"


struct AVPacket;
struct AVStream;

namespace Donut
{

	class DONUT_API DonutAVDemuxer : public DonutAVFormatBase
	{
	public:
		AVFormatContext* openContext(const char* url);
		int readPacket(AVPacket* pkt);

		int seekByPts(long long pts, int stream_index);

		int64_t getTotalDuration();
		//DonutAVRational getFrameRate();
	};
}

#endif
