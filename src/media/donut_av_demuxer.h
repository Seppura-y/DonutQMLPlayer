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
		int64_t getTotalDuration(int64_t& total, int64_t& audio, int64_t& video);
		//DonutAVRational getFrameRate();
		
		AVStream* getVideoStream(int index);
		AVStream* getAudioStream(int index);

		AVStream** getVideoStreams() { return video_streams_; }
		AVStream** getAudioStreams() { return audio_streams_; }

		int64_t getStreamStartTime(int stream_index);
		int64_t getStartTime();

		int seekFile(int64_t min, int64_t target, int64_t max, int flags);

		void setPaused(bool paused);
	private:
		int vs_count_ = 0;
		int as_count_ = 0;

		AVStream* video_streams_[8]{ nullptr };
		AVStream* audio_streams_[8]{ nullptr };

		AVRational video_timebase_ = { 0 };
		AVRational audio_timebase_ = { 0 };

		int64_t total_duration_ = 0;
		int64_t video_duration_ = 0;
		int64_t audio_duration_ = 0;

		bool is_paused_ = false;
	};
}

#endif
