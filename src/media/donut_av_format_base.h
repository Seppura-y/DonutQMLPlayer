#ifndef DONUT_AV_FORMAT_BASE_H
#define DONUT_AV_FORMAT_BASE_H


#include <mutex>
#include "core.h"
#include "donut_av_datas.h"
#include "donut_av_timer.h"

struct AVFormatContext;
struct AVRational;
struct AVStream;
struct AVCodecParameters;
struct AVCodecContext;


namespace Donut
{
class DONUT_API DonutAVFormatBase
{
public:
	DonutAVFormatBase();
	virtual ~DonutAVFormatBase();

	int setFormatContext(AVFormatContext* ctx);
	int closeContext();

	int copyCodecParameters(int stream_index, AVCodecParameters* dst);
	int copyCodecParameters(int stream_index, AVCodecContext* dst);

	std::shared_ptr<DonutAVParamWarpper> copyVideoParameters();
	std::shared_ptr<DonutAVParamWarpper> copyAudioParameters();

	AVCodecParameters* copyRawVideoParameters();

	bool hasVideo() { return video_index_ >= 0; };
	bool hasAudio() { return audio_index_ >= 0; };

	DonutAVRational getVideoFramerate();

	int getAudioIndex() { return audio_index_; }
	int getVideoIndex() { return video_index_; }
	void getVideoTimebase(AVRational* rational);
	void getAudioTimebase(AVRational* rational);

	int getVideoCodecId() { return video_codec_id_; }
	int getAudioCodecId() { return audio_codec_id_; }

	int timeScale(int index, AVPacket* pkt, AVRational src, long long pts);

	void setTimeoutThreshold(int timeout);

	bool isNetworkConnected() { return is_network_connected_; }


	int getCodecExtraData(uint8_t* buffer, int& size);

protected:
	static int timeoutCallback(void* opaque);
	bool isTimeout();

protected:
	std::mutex mtx_;

	AVFormatContext* fmt_ctx_ = nullptr;

	int audio_index_ = -1;
	int video_index_ = -1;
	AVRational* audio_src_timebase_ = nullptr;
	AVRational* video_src_timebase_ = nullptr ;

	int video_codec_id_ = 0;
	int audio_codec_id_ = 0;

	bool is_network_connected_ = false;
	bool is_timeout_enabled_ = true;
	int timeout_threshold_ = -1;
	int last_proc_time_ = -1;
};

}

#endif