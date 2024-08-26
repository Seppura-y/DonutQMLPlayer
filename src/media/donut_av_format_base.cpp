#include "donut_av_format_base.h"

extern"C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include <iostream>

#include "log.h"

namespace Donut
{
DonutAVFormatBase::DonutAVFormatBase()
{
	if (!audio_src_timebase_)
	{
		audio_src_timebase_ = new AVRational();
	}
	if (!video_src_timebase_)
	{
		video_src_timebase_ = new AVRational();
	}
}

DonutAVFormatBase::~DonutAVFormatBase()
{
	if (audio_src_timebase_)
	{
		delete audio_src_timebase_;
		audio_src_timebase_ = nullptr;
	}
	if (video_src_timebase_)
	{
		delete video_src_timebase_;
		video_src_timebase_ = nullptr;
	}
}

int DonutAVFormatBase::setFormatContext(AVFormatContext* ctx)
{
	closeContext();
	std::unique_lock<std::mutex> lock(mtx_);
	fmt_ctx_ = ctx;
	if (fmt_ctx_ == nullptr)
	{
		is_network_connected_ = false;
		return 0;
	}
	is_network_connected_ = true;
	last_proc_time_ = GetCurrentTimeMsec();
	if (timeout_threshold_ > 0)
	{
		AVIOInterruptCB callback = { timeoutCallback, this };
		fmt_ctx_->interrupt_callback = callback;
	}

	audio_index_ = -1;
	video_index_ = -1;
	for (int i = 0; i < fmt_ctx_->nb_streams; i++)
	{
		if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_index_ = fmt_ctx_->streams[i]->index;
			audio_codec_id_ = fmt_ctx_->streams[i]->codecpar->codec_id;
			audio_src_timebase_->den = fmt_ctx_->streams[i]->time_base.den;
			audio_src_timebase_->num = fmt_ctx_->streams[i]->time_base.num;
		}

		if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_index_ = i;
			video_codec_id_ = fmt_ctx_->streams[i]->codecpar->codec_id;
			video_src_timebase_->den = fmt_ctx_->streams[i]->time_base.den;
			video_src_timebase_->num = fmt_ctx_->streams[i]->time_base.num;
			
			// If demux, get sps pps data
			if (fmt_ctx_->iformat)
			{

			}
		}
	}
	return 0;
}

int DonutAVFormatBase::closeContext()
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (fmt_ctx_)
	{
		if (fmt_ctx_->oformat)
		{
			/* -muxing: set by the user before avformat_write_header().The caller must
			* take care of closing / freeing the IO context.
			*/
			if (fmt_ctx_->pb)
			{
				avio_closep(&fmt_ctx_->pb);
			}
			avformat_free_context(fmt_ctx_);
		}
		else if (fmt_ctx_->iformat)
		{
			avformat_close_input(&fmt_ctx_);
		}
		else
		{
			avformat_free_context(fmt_ctx_);
		}
		fmt_ctx_ = nullptr;
	}

	return 0;
}

int DonutAVFormatBase::copyCodecParameters(int stream_index, AVCodecParameters* dst)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return -1;
	}
	if (stream_index < 0 || stream_index > fmt_ctx_->nb_streams)
	{
		return -1;
	}
	int ret = avcodec_parameters_copy(dst, fmt_ctx_->streams[stream_index]->codecpar);
	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

int DonutAVFormatBase::copyCodecParameters(int stream_index, AVCodecContext* dst)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return -1;
	}
	if (stream_index < 0 || stream_index > fmt_ctx_->nb_streams)
	{
		return -1;
	}
	int ret = avcodec_parameters_to_context(dst, fmt_ctx_->streams[stream_index]->codecpar);
	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

std::shared_ptr<DonutAVParamWarpper> DonutAVFormatBase::copyVideoParameters()
{
	std::shared_ptr<DonutAVParamWarpper> param = std::make_shared<DonutAVParamWarpper>();
	std::unique_lock<std::mutex> lock(mtx_);
	if (!hasVideo() || !fmt_ctx_)
	{
		return {};
	}
	avcodec_parameters_copy(param->para, fmt_ctx_->streams[video_index_]->codecpar);
	*param->time_base = fmt_ctx_->streams[video_index_]->time_base;

	// ²Î¿¼ffplayµÄvideo_thread
	param->framerate_ = av_guess_frame_rate(fmt_ctx_, fmt_ctx_->streams[video_index_], nullptr);
	return param;
}

std::shared_ptr<DonutAVParamWarpper> DonutAVFormatBase::copyAudioParameters()
{
	std::shared_ptr<DonutAVParamWarpper> param = std::make_shared<DonutAVParamWarpper>();
	std::unique_lock<std::mutex> lock(mtx_);
	if (!hasAudio() || !fmt_ctx_)
	{
		return {};
	}
	avcodec_parameters_copy(param->para, fmt_ctx_->streams[audio_index_]->codecpar);
	//*param->time_base = {1, fmt_ctx_->streams[audio_index_]->codecpar->sample_rate};
	*param->time_base = fmt_ctx_->streams[audio_index_]->time_base;
	//*param->time_base = fmt_ctx_->streams[audio_index_]->time_base;
	param->samplerate_ = { 1, fmt_ctx_->streams[audio_index_]->codecpar->sample_rate };
	//*param->time_base = { 1, fmt_ctx_->streams[audio_index_]->codecpar->sample_rate };
	return param;
}

AVCodecParameters* DonutAVFormatBase::copyRawVideoParameters()
{
	std::lock_guard<std::mutex> lock(mtx_);
	if (!hasVideo() || !fmt_ctx_)
	{
		return nullptr;
	}
	AVCodecParameters* param = avcodec_parameters_alloc();
	avcodec_parameters_copy(param, fmt_ctx_->streams[video_index_]->codecpar);
	return param;
}

DonutAVRational DonutAVFormatBase::getVideoFramerate()
{
	if (hasVideo())
	{
		AVRational framerate = av_guess_frame_rate(fmt_ctx_, fmt_ctx_->streams[video_index_], nullptr);
		return DonutAVRational(framerate.num, framerate.den);
		//return DonutAVRational(fmt_ctx_->streams[video_index_]->r_frame_rate.num,
		//	fmt_ctx_->streams[video_index_]->r_frame_rate.den);
	}
	else
	{
		return DonutAVRational(0, 0);
	}

}

void DonutAVFormatBase::getVideoTimebase(AVRational* rational)
{
	rational->num = video_src_timebase_->num;
	rational->den = video_src_timebase_->den;
}

void DonutAVFormatBase::getAudioTimebase(AVRational* rational)
{
	rational->num = audio_src_timebase_->num;
	rational->den = audio_src_timebase_->den;
}

int DonutAVFormatBase::timeScale(int index, AVPacket* pkt, AVRational src, long long pts)
{
	if (!pkt)
	{
		DN_CORE_WARN("timeScale failed : (!pkt)");
		return -1;
	}
	//std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		DN_CORE_WARN("timeScale failed : (!fmt_ctx_)");
		return -1;
	}
	AVStream* stream = fmt_ctx_->streams[index];
	pkt->pts = av_rescale_q_rnd(pkt->pts - pts, src, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->dts = av_rescale_q_rnd(pkt->dts - pts, src, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->duration = av_rescale_q_rnd(pkt->duration, src, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	pkt->pos = -1;
	return 0;
}

void DonutAVFormatBase::setTimeoutThreshold(int timeout)
{
	std::unique_lock<std::mutex> lock(mtx_);
	this->timeout_threshold_ = timeout;
	if (fmt_ctx_)
	{
		AVIOInterruptCB callback = { timeoutCallback, this };
		fmt_ctx_->interrupt_callback = callback;
	}
}

int DonutAVFormatBase::getCodecExtraData(uint8_t* buffer, int& size)
{
	std::unique_lock<std::mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		std::cout << "GetCodecExtraData failed : fmt_ctx_ is null" << std::endl;
		return -1;
	}
	if (fmt_ctx_->streams[video_index_]->codecpar->codec_id == AV_CODEC_ID_H264)
	{
		memcpy(buffer, fmt_ctx_->streams[video_index_]->codecpar->extradata, fmt_ctx_->streams[video_index_]->codecpar->extradata_size);
		size = fmt_ctx_->streams[video_index_]->codecpar->extradata_size;
	}
	else
	{
		return -1;
	}
	return 0;
}

int DonutAVFormatBase::timeoutCallback(void* opaque)
{
	DonutAVFormatBase* fmt = static_cast<DonutAVFormatBase*>(opaque);
	if (fmt->isTimeout())
	{
		std::cout << "timeout occur" << std::endl;
		return 1;
	}
	return 0;
}

bool DonutAVFormatBase::isTimeout()
{
	int64_t tmp = GetCurrentTimeMsec();
	if (is_timeout_enabled_ && (tmp - last_proc_time_ >= timeout_threshold_))
	{
		is_network_connected_ = false;
		last_proc_time_ = GetCurrentTimeMsec();
		return true;
	}
	return false;
}

}
