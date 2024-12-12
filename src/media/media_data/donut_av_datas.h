#ifndef DONUT_AV_DATAS_H
#define DONUT_AV_DATAS_H

extern"C"
{
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}

#include <list>
#include <iostream>
#include <string>

#include <mutex>

#include "core.h"

struct AVFrame;
struct AVPacket;
struct AVCodecParameters;
struct AVRational;

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30


#define MIN_LOG_LEVEL Log_Info
#define LOG(s,level) if(level >= Log_Info)\
std::cout<<"level_" << level<<":"<<"file : "<<__FILE__<<" "<<"line : "<<__LINE__<<": "\
<<s<<"\n"<<std::endl;

#define LOGINFO(s)     LOG(s,Log_Info)
#define LOGDEBUG(s)  LOG(s,Log_Debug)
#define LOGERROR(s)   LOG(s,Log_Error)
#define LOGFATAL(s)	LOG(s,Log_Fatal)

#define GET_AV_STRERR(ret,str) {\
	char buffer[1024] = {0};\
	av_strerror(ret,buffer,sizeof(buffer));\
	str = std::string(buffer);\
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_U8        0x0008  /**< Unsigned 8-bit samples */
#define AUDIO_S8        0x8008  /**< Signed 8-bit samples */
#define AUDIO_U16LSB    0x0010  /**< Unsigned 16-bit samples */
#define AUDIO_S16LSB    0x8010  /**< Signed 16-bit samples */
#define AUDIO_U16MSB    0x1010  /**< As above, but big-endian byte order */
#define AUDIO_S16MSB    0x9010  /**< As above, but big-endian byte order */
#define AUDIO_U16       AUDIO_U16LSB
#define AUDIO_S16       AUDIO_S16LSB
/* @} */

/**
 *  \name int32 support
 */
 /* @{ */
#define AUDIO_S32LSB    0x8020  /**< 32-bit integer samples */
#define AUDIO_S32MSB    0x9020  /**< As above, but big-endian byte order */
#define AUDIO_S32       AUDIO_S32LSB
/* @} */

/**
 *  \name float32 support
 */
 /* @{ */
#define AUDIO_F32LSB    0x8120  /**< 32-bit floating point samples */
#define AUDIO_F32MSB    0x9120  /**< As above, but big-endian byte order */
#define AUDIO_F32       AUDIO_F32LSB
/* @} */

/**
 *  \name Native audio byte ordering
 */
 /* @{ */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define AUDIO_U16SYS    AUDIO_U16LSB
#define AUDIO_S16SYS    AUDIO_S16LSB
#define AUDIO_S32SYS    AUDIO_S32LSB
#define AUDIO_F32SYS    AUDIO_F32LSB
#else
#define AUDIO_U16SYS    AUDIO_U16MSB
#define AUDIO_S16SYS    AUDIO_S16MSB
#define AUDIO_S32SYS    AUDIO_S32MSB
#define AUDIO_F32SYS    AUDIO_F32MSB
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Donut
{
	//enum class DONUT_API PixFormat
	//{
	//	PIX_FORMAT_YUV420P = 0,
	//	PIX_FORMAT_NV12 = 23,
	//	PIX_FORMAT_ARGB = 25,
	//	PIX_FORMAT_RGBA = 26,
	//	PIX_FORMAT_BGRA = 28
	//};

	//enum class DONUT_API RenderType
	//{
	//	RENDER_TYPE_SDL = 0,
	//	RENDER_TYPE_OPENGL
	//};

	//enum class DONUT_API LogLevel
	//{
	//	Log_Info,
	//	Log_Debug,
	//	Log_Error,
	//	Log_Fatal
	//};

	struct AudioSpec
	{
		int sample_rate = 44100;
		enum AVSampleFormat av_fmt = AV_SAMPLE_FMT_S16;
		int64_t ch_layout = 0;
		unsigned short sdl_fmt = AUDIO_S16SYS;
		unsigned char channels = 2;
		unsigned char sample_size = 2;
		unsigned short samples = 1024;

		int frame_size = 0;
		int bytes_per_sec = 0;

		bool operator==(AudioSpec& other)
		{
			return this->sample_rate == other.sample_rate &&
				this->av_fmt == other.av_fmt &&
				this->ch_layout == other.ch_layout &&
				this->sdl_fmt == other.sdl_fmt &&
				this->channels == other.channels &&
				this->sample_size == other.sample_size &&
				this->samples == other.samples &&
				this->frame_size == other.frame_size &&
				this->bytes_per_sec == other.bytes_per_sec;
		}
	};

	int64_t GetCurrentTimeMsec();
	//void SleepForMsec(int ms);
	//void FreeFrame(AVFrame** frame);

	class DONUT_API DonutAVParamWarpper
	{
	public:
		AVCodecParameters* para = nullptr;
		AVRational* time_base = nullptr;
		AVRational framerate_ = { 0 };
		AVRational samplerate_ = { 0 };
		static DonutAVParamWarpper* create();
		~DonutAVParamWarpper();
		//private:
		DonutAVParamWarpper();

		DonutAVParamWarpper(const DonutAVParamWarpper&) = delete;
		DonutAVParamWarpper operator=(const DonutAVParamWarpper&) = delete;
	};

	//class DONUT_API DonutAVPacketDataList
	//{
	//public:

	//	void push(AVPacket* packet);
	//	AVPacket* pop();
	//	void clear();
	//private:
	//	std::list<AVPacket*> pkt_list_;
	//	int max_list_ = 500;
	//	std::mutex mtx_;
	//};

	//class DONUT_API DonutAVFrameDataList
	//{
	//public:
	//	void push(AVFrame* frame);
	//	AVFrame* pop();
	//	void clear();

	//private:
	//	std::list<AVFrame*> frm_list_;
	//	int max_list_ = 500;
	//	std::mutex mtx_;
	//};


	//class DONUT_API DonutAVCachedPacketDataList
	//{
	//public:

	//	void push(AVPacket* packet);
	//	AVPacket* pop();
	//	void clear();
	//	int size();
	//private:
	//	std::list<AVPacket*> pkt_list_;
	//	int max_list_ = 500;
	//	std::mutex mtx_;
	//};



}

#endif
