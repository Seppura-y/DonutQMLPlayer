#ifndef DONUT_AV_DEMUX_HANDLER_H
#define DONUT_AV_DEMUX_HANDLER_H

#include "i_donut_av_base_handler.h"
#include "donut_av_demuxer.h"
#include "donut_av_datas.h"

#include <functional>
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

		AVCodecParameters* copyRawVideoParameters();

		bool hasVideo() { return has_video_; }
		bool hasAudio() { return has_audio_; }

		int getVideoIndex() { return video_index_; }
		int getAudioIndex() { return audio_index_; }

		AVStream* getVideoStream(int index);
		AVStream* getAudioStream(int index);

		AVStream** getVideoStreams() { return demuxer_.getVideoStreams(); }
		AVStream** getAudioStreams() { return demuxer_.getAudioStreams(); }
		// h264 sps pps
		int copyCodecExtraData(uint8_t* buffer, int& size);

		void seekByTimePos(double value);
		int64_t getTotalDuration() { return total_duration_; };

		DonutAVRational getVideoFramerate();

		using eof_callback = std::function<void(void)>;

		void setEofCallback(eof_callback cb) { eof_cb_ = cb; }


		void setAudioQueue(std::shared_ptr<DonutAVPacketQueue>& audio_queue);
		void setVideoQueue(std::shared_ptr<DonutAVPacketQueue>& video_queue);

		void setPaused(bool pause);
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
		int64_t audio_duration_ = -1;
		int64_t video_duration_ = -1;
		int frame_rate_ = -1;

		AVStream** video_streams_ = nullptr;
		AVStream** audio_streams_ = nullptr;

		eof_callback eof_cb_;

		std::shared_ptr<DonutAVPacketQueue> a_packet_queue_;
		std::shared_ptr<DonutAVPacketQueue> v_packet_queue_;

		int64_t seek_pos_ = 0;
		int64_t seek_delta_ = 0;
		int seek_req_ = 0;
		int seek_flags_ = 0;
	};

}

#endif