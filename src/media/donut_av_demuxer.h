#ifndef DONUT_AV_DEMUXER_H
#define DONUT_AV_DEMUXER_H

//#include "core.h"
#include "donut_av_format_base.h"
#include "donut_av_packet.h"

struct AVPacket;
struct AVStream;
struct AVFormatContext;

namespace Donut
{

	class DonutAVDemuxer : public DonutAVFormatBase
	{
	public:
		AVFormatContext* openContext(const char* url);
		int readPacket(AVPacket* pkt);
		int readPacket(std::shared_ptr<DonutAVPacket>& pkt);
		int readPacket(std::shared_ptr<DonutAVPacketQueue>& pkt_queue);
		int seekByPts(long long pts, int stream_index);

		int64_t getTotalDuration();
		//DonutAVRational getFrameRate();
	};
}

#endif
