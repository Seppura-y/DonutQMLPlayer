#include "donut_av_demux_handler.h"

extern"C"
{
#include <libavformat/avformat.h>
}

namespace Donut
{


	Donut::DonutAVDemuxHandler::DonutAVDemuxHandler()
	{
	}

	Donut::DonutAVDemuxHandler::~DonutAVDemuxHandler()
	{
	}

	int Donut::DonutAVDemuxHandler::openAVSource(const char* url, int timeout)
	{
		this->url_ = url;
		this->timeout_threshold_ = timeout;

		demuxer_.closeContext();

		AVFormatContext* fmt_ctx = demuxer_.openContext(url_.c_str());
		if (!fmt_ctx)
		{
			//DN_CORE_ERROR("demuxer_.openContext failed");

			return -1;
		}

		demuxer_.setFormatContext(fmt_ctx);
		demuxer_.setTimeoutThreshold(timeout_threshold_);

		has_audio_ = demuxer_.hasAudio();
		has_video_ = demuxer_.hasVideo();

		audio_index_ = demuxer_.getAudioIndex();
		video_index_ = demuxer_.getVideoIndex();

		this->total_duration_ = demuxer_.getTotalDuration();

		video_streams_ = demuxer_.getVideoStreams();
		audio_streams_ = demuxer_.getAudioStreams();

		return 0;
	}

	std::shared_ptr<DonutAVParamWarpper> Donut::DonutAVDemuxHandler::copyVideoParameters()
	{
		return demuxer_.copyVideoParameters();
	}

	std::shared_ptr<DonutAVParamWarpper> Donut::DonutAVDemuxHandler::copyAudioParameters()
	{
		return demuxer_.copyAudioParameters();
	}

	AVCodecParameters* DonutAVDemuxHandler::copyRawVideoParameters()
	{
		return demuxer_.copyRawVideoParameters();
	}

	AVStream* DonutAVDemuxHandler::getVideoStream(int index)
	{
		return video_streams_[index];
	}

	AVStream* DonutAVDemuxHandler::getAudioStream(int index)
	{
		return audio_streams_[index];
	}

	int Donut::DonutAVDemuxHandler::copyCodecExtraData(uint8_t* buffer, int& size)
	{
		return 0;
	}

	int Donut::DonutAVDemuxHandler::seek(long long pts)
	{
		return 0;
	}

	int Donut::DonutAVDemuxHandler::getVideoFramerate()
	{
		return 0;
	}

	void DonutAVDemuxHandler::setAudioQueue(std::shared_ptr<DonutAVPacketQueue>& audio_queue)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		this->a_packet_queue_ = audio_queue;
	}

	void DonutAVDemuxHandler::setVideoQueue(std::shared_ptr<DonutAVPacketQueue>& video_queue)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		this->v_packet_queue_ = video_queue;
	}

	void Donut::DonutAVDemuxHandler::threadLoop()
	{
		AVPacket* demux_pkt = av_packet_alloc();
		while (!is_exit_)
		{
			if (is_pause_)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			if (
				(hasVideo() && v_packet_queue_ && v_packet_queue_->packetQueueHasEnoughPackets())
				||
				(hasAudio() && a_packet_queue_ && a_packet_queue_->packetQueueHasEnoughPackets())
				)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			if (int ret = demuxer_.readPacket(demux_pkt) != 0)
			{
				if (ret == AVERROR(EOF))
				{
					eof_cb_();
				}

				if (!demuxer_.isNetworkConnected())
				{
					openAVSource(url_.c_str(), timeout_threshold_);
				}

				av_packet_unref(demux_pkt);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			
			if (handler_nodes_.size() != 0)
				notify(demux_pkt);
			else
				av_packet_unref(demux_pkt);
		}

		av_packet_free(&demux_pkt);
	}

}