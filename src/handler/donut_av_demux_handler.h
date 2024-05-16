#ifndef DONUT_AV_DEMUX_HANDLER_H
#define DONUT_AV_DEMUX_HANDLER_H

#include "i_donut_av_base_handler.h"
#include "donut_av_demuxer.h"
#include "donut_av_datas.h"
namespace Donut
{
	class DonutAVDemuxHandler : public IDonutAVBaseHandler
	{
	public:
		DonutAVDemuxHandler();
		~DonutAVDemuxHandler();
		int openAVSource(const char* url, int timeout = 1000);

		//virtual void handle(void* package) override;
		//virtual void handle(std::shared_ptr<DonutAVPacketQueue>& pkt_queue, std::shared_ptr<DonutAVFrameQueue>& frame_queue) override {}

		std::shared_ptr<DonutAVParamWarpper> copyVideoParameters();
		std::shared_ptr<DonutAVParamWarpper> copyAudioParameters();

		bool hasVideo() { return has_video_; }
		bool hasAudio() { return has_audio_; }

		// h264 sps pps
		int copyCodecExtraData(uint8_t* buffer, int& size);

		int seek(long long pts);
		int64_t getTotalDuration() { return total_duration_; };

		int getVideoFramerate();
	protected:
		virtual void threadLoop() override;
	private:
		DonutAVDemuxer demuxer_;
		std::string url_;
		int timeout_threshold_ = 0; // ms

		bool has_video_ = false;
		bool has_audio_ = false;
		int video_index_ = -1;
		int audio_index_ = -1;

		int64_t total_duration_ = -1;
		int frame_rate_ = -1;
	};

}

#endif