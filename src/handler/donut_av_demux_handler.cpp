#include "donut_av_demux_handler.h"

#include "donut_av_global.h"
#include "log.h"

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

		//this->total_duration_ = demuxer_.getTotalDuration();
		demuxer_.getTotalDuration(total_duration_, audio_duration_, video_duration_);

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

	void DonutAVDemuxHandler::seekByTimePos(double value)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		//int64_t ts = value * audio_duration_;
		int64_t ts = value * total_duration_ * 1000000LL;
		int64_t start_time = demuxer_.getStartTime();
		if ( start_time != AV_NOPTS_VALUE)
		{
			ts += start_time;
		}
		seek_pos_ = ts;
		seek_delta_ = 0;
		seek_flags_ &= ~AVSEEK_FLAG_BYTE;
		seek_req_ = 1;
	}

	DonutAVRational Donut::DonutAVDemuxHandler::getVideoFramerate()
	{
		return demuxer_.getVideoFramerate();
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

	void DonutAVDemuxHandler::setPaused(bool pause)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		demuxer_.setPaused(pause);
		is_paused_ = pause;
	}

	void Donut::DonutAVDemuxHandler::threadLoop()
	{
		AVPacket* demux_pkt = av_packet_alloc();
		while (!is_exit_)
		{
			{
				std::lock_guard<std::mutex> lock(mtx_);
				if (is_paused_)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					continue;
				}
			}

			if (seek_req_)
			{
				int64_t seek_target = seek_pos_;
				int64_t seek_min = seek_delta_ > 0 ? seek_target - seek_delta_ + 2 : INT64_MIN;
				int64_t seek_max = seek_delta_ < 0 ? seek_target - seek_delta_ - 2 : INT64_MAX;

				int ret = demuxer_.seekFile(seek_min, seek_target, seek_max, seek_flags_);
				//int ret = demuxer_.seekByPts(seek_target, audio_index_);
				if (ret < 0)
				{

				}
				else
				{
					if (hasAudio())
					{
						a_packet_queue_->packetQueueFlush();
						a_packet_queue_->packetQueuePut(DonutAVPacket::flush_packet_);
					}

					if (hasVideo())
					{
						v_packet_queue_->packetQueueFlush();
						v_packet_queue_->packetQueuePut(DonutAVPacket::flush_packet_);
					}
				}
				seek_req_ = 0;

				continue;
			}

			if (
				// condition 1
				(a_packet_queue_->getSize() + v_packet_queue_->getSize() > MAX_QUEUE_SIZE) ||

				// condition 2
				(hasVideo() && v_packet_queue_ && v_packet_queue_->packetQueueHasEnoughPackets())
				&&
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


			if (demux_pkt->stream_index == video_index_)
			{
				auto dn_pkt = std::make_shared<DonutAVPacket>(demux_pkt);

				//if (!demux_pkt->data[0])
				//{
				//	DN_CORE_ERROR("data[0] empty");
				//}
				//if (!demux_pkt->data[1])
				//{
				//	DN_CORE_ERROR("data[1] empty");
				//}
				//if (!demux_pkt->data[2])
				//{
				//	DN_CORE_ERROR("data[2] empty");
				//}
				v_packet_queue_->packetQueuePut(dn_pkt);
			}
			else if (demux_pkt->stream_index == audio_index_)
			{
				auto dn_pkt = std::make_shared<DonutAVPacket>(demux_pkt);
				a_packet_queue_->packetQueuePut(dn_pkt);
			}

			av_packet_unref(demux_pkt);

			
			//if (handler_nodes_.size() != 0)
			//{
			//	notify(demux_pkt);
			//}
			//else
			//{
			//	av_packet_unref(demux_pkt);
			//}
		}

		av_packet_free(&demux_pkt);
	}

}