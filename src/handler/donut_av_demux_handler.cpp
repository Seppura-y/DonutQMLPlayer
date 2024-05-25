#include "donut_av_demux_handler.h"

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

		return 0;
	}

	std::shared_ptr<DonutAVParamWarpper> Donut::DonutAVDemuxHandler::copyVideoParameters()
	{
		return std::shared_ptr<DonutAVParamWarpper>();
	}

	std::shared_ptr<DonutAVParamWarpper> Donut::DonutAVDemuxHandler::copyAudioParameters()
	{
		return std::shared_ptr<DonutAVParamWarpper>();
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

	void Donut::DonutAVDemuxHandler::threadLoop()
	{
	}

}